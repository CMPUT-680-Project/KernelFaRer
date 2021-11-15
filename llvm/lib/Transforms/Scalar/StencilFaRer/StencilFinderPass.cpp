//===- GEMMReplacer.cpp - Matrix-Multiply Replacer Pass ---------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This pass replaces matrix-multiply loops previously recognized by the matcher
// into llvm.matrix.multiply.* intrinsic calls. In cases that this
// kicks in, it can be a significant performance win.
//
//===----------------------------------------------------------------------===//
// 
// TODO List:
// * Add command-line replacement options for GEMM
//
//===----------------------------------------------------------------------===//

#include "llvm/Analysis/ScalarEvolution.h"
#include "llvm/IR/CFG.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/MatrixBuilder.h"
#include "llvm/IR/PassManager.h"
#include "llvm/Support/Alignment.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Transforms/Scalar/StencilFaRer.h"
#include "llvm/Transforms/Utils/BasicBlockUtils.h"

#define DEBUG_TYPE "stencil-finder-pass"

using namespace llvm;
using namespace StencilFaRer;

// Anonymous namespace containing rewriter functions.
namespace {

// Constants are from Eigen enum values.
// https://gitlab.com/libeigen/eigen/-/blob/master/Eigen/src/Core/util/Constants.h#L316
constexpr int32_t EigenColMaj = 0;
constexpr int32_t EigenRowMaj = 1;

constexpr unsigned int EigenMaxArgs = 14;
constexpr unsigned int EigenMaxFNameLen = 13;
constexpr size_t EigenSizeWidth = 64;

// A helper function that returns Matrix loaded in column-major order as a
// flat-vector.
Value *loadMatrixToFlatVector(MatrixBuilder<IRBuilder<>> &MBuilder,
                              Value &Matrix, Value &Rows, Value &Columns,
                              Value &Stride, bool IsColMajor,
                              const Align &Alignment) {
  Value *Vec;
  uint64_t RowsAsUInt64 = cast<ConstantInt>(Rows).getZExtValue();
  uint64_t ColsAsUInt64 = cast<ConstantInt>(Columns).getZExtValue();
  if (IsColMajor)
    Vec = MBuilder.CreateColumnMajorLoad(&Matrix, Alignment, &Stride,
                                         /*isVolatile*/ false, RowsAsUInt64,
                                         ColsAsUInt64);
  else
    Vec = MBuilder.CreateMatrixTranspose(
        MBuilder.CreateColumnMajorLoad(&Matrix, Alignment, &Stride,
                                       /*isVolatile*/ false, ColsAsUInt64,
                                       RowsAsUInt64),
        ColsAsUInt64, RowsAsUInt64);

  return Vec;
}

// A helper function that stores column-major order Matrix into Dest as either
// column or row-major order depending on isColMajor value.
void storeFlatVectorMatrix(MatrixBuilder<IRBuilder<>> &MBuilder, Value &Matrix,
                           Value &Dest, Value &Rows, Value &Columns,
                           Value &Stride, bool IsColMajor,
                           const Align &Alignment) {
  uint64_t RowsAsUInt64 = cast<ConstantInt>(Rows).getZExtValue();
  uint64_t ColsAsUInt64 = cast<ConstantInt>(Columns).getZExtValue();
  if (IsColMajor)
    MBuilder.CreateColumnMajorStore(&Matrix, &Dest, Align(Alignment), &Stride,
                                    /*isVolatile*/ false, RowsAsUInt64,
                                    ColsAsUInt64);
  else
    MBuilder.CreateColumnMajorStore(
        MBuilder.CreateMatrixTranspose(&Matrix, RowsAsUInt64, ColsAsUInt64),
        &Dest, Align(Alignment), &Stride, /*isVolatile*/ false, ColsAsUInt64,
        RowsAsUInt64);
}

/// A helper function to retrieve the scalar type of a value pointer.
///
/// \param M a value pointer to a scalar type or a 2D array of scalar values
///
/// \returns the scalar type of a value pointer to 2D array or scalar pointed by
/// \p M.
Type *getMatrixElementType(const Value &M) {
  auto *ElementType = M.getType()->getPointerElementType();
  if (ElementType->isArrayTy()) {
    ElementType = ElementType->getArrayElementType();
    if (ElementType->isArrayTy())
      ElementType = ElementType->getArrayElementType();
  }
  assert(ElementType->isIntegerTy() || ElementType->isFloatingPointTy());
  return ElementType;
}

/// A helper function that Down-/uppercasts integer value to Int32
///
/// \param V a Value pointer to an integer type
/// \param downcast this value is set to true if the value \p V was downcast
///
/// \returns the downcast value
auto *prepBLASInt32(IRBuilder<> &IR, Value *V, bool &Downcast) {
  if (V->getType()->getIntegerBitWidth() > 32)
    Downcast |= true; // We're doing a potentially (but unlikely) illegal cast.
  if (V->getType()->getIntegerBitWidth() != 32)
    V = IR.CreateIntCast(V, IR.getInt32Ty(), false);
  return V;
}

/// A helper function that returns a constant scalar value of 1 if it was not
/// matched (alpha and beta implicitly equal to 1).
///
/// \param V a Value that points to a scalar or nullptr
/// \param opTy the Type of the scalar pointed by \p
///
/// \returns \p V if it not nullptr. Otherwise, returns a constant equal to 1.
Value *prepBLASScalar(IRBuilder<> &IR, Value *V, Type *OpTy, double Init = 1.) {
  Value *Scalar;
  if (OpTy->isFloatTy()) {
    float f = Init;
    Scalar = ConstantFP::get(OpTy, APFloat(f));
  } else if (OpTy->isDoubleTy()) {
    double d = Init;
    Scalar = ConstantFP::get(OpTy, APFloat(d));
  } else {
    llvm_unreachable("Scalar needs to be either FloatTy or DoubleTy.");
  }
  return V != nullptr ? V : Scalar;
}

/// A helper function that uppercasts integer value to Int64 if needed
///
/// \param V a Value pointer to an integer value
///
/// \returns \p V or the uppercasted \p V
auto *prepEigenInt64(IRBuilder<> &IR, Value *const &V) {
  if (!V->getType()->isIntegerTy(EigenSizeWidth)) {
    return IR.CreateIntCast(V, IR.getIntNTy(EigenSizeWidth), false);
  }
  return V;
}

/// A helper function that returns the base pointer to matrix \p M. If the
/// base pointer points to a vector of pointers it needs to be casted to a
/// flat-pointer before it is passed to cblas_X()
///
/// \param IR the IR builder handling the current function
/// transformation
///
/// \param M the matrix from which the base pointer will be returned
///
/// \returns the base pointer to matrix \p M.
auto *getFlatPointerToMatrix(IRBuilder<> &IR, const StencilFaRer::Matrix &M) {
  // Flatten array
  auto *BasePtr = &M.getBaseAddressPointer();
  auto *DestTy = getMatrixElementType(*BasePtr)->getPointerTo();
  BasePtr = IR.CreateBitCast(BasePtr, DestTy);

  // If we have a pointer to a vector (e.g. [1024 x double]*) it is safe to
  // convert it to a pointer to the base type. We cast away explicit size
  // info but BLAS doesn't care.
  if (auto *MATy = dyn_cast<ArrayType>(getMatrixElementType(*BasePtr)))
    BasePtr = IR.CreatePointerCast(BasePtr,
                                   MATy->getArrayElementType()->getPointerTo());
  return BasePtr;
}

inline void insertNoInlineCall(Module &M, IRBuilder<> &IR,
                               ArrayRef<Type *> ArgTys, ArrayRef<Value *> Args,
                               StringRef FunctionName) {
  // Add a declaration for the function we're going to be replacing with.
  auto *FTy = FunctionType::get(IR.getVoidTy(), ArgTys, false);
  FunctionCallee F = M.getOrInsertFunction(FunctionName, FTy);

  // We can never inline this call.
  cast<Function>(F.getCallee())->addFnAttr(Attribute::NoInline);

  // Create the call to the function.
  CallInst *Call = IR.CreateCall(F, Args);
  Call->setIsNoInline();
}

} // End anonymous namespace.

namespace StencilFaRer {

// Replaces the corresponding basic blocks of MatMul IR with a call to
// llvm.matrix.multiply.
bool runImpl(Function &F, GEMMMatcher::Result &GMPR) {
  // Have we changed in this function?
  bool Changed = false;

  // Iterate through all GEMMs found.
  for (std::unique_ptr<Kernel> &Ker : *GMPR) {
    LLVM_DEBUG(dbgs() << "Kernel at line "
                      << Ker->getAssociatedLoop().getStartLoc().getLine()
                      << '\n');
    // ! TODO: Update log to be stencil
  }

  return Changed;
}

} // end of namespace StencilFaRer

/// Replaces Matrix Multiply occurencies with calls to llvm.matrix.multiply
// Runs on each function, makes a list of candidates and updates their IR when
// the change is possible
PreservedAnalyses StencilFinderPass::run(Function &F,
                                        FunctionAnalysisManager &FAM) {
  LLVM_DEBUG(dbgs() << "Function: ");
  F.print(dbgs());
  LoopInfo &LI = FAM.getResult<LoopAnalysis>(F);
  DominatorTree &DT = FAM.getResult<DominatorTreeAnalysis>(F);
  ScalarEvolution &SE = FAM.getResult<ScalarEvolutionAnalysis>(F);
  StencilFaRer::GEMMMatcher::Result GMPR = StencilFaRer::GEMMMatcher::run(F, LI, DT, SE);
  bool Changed = runImpl(F, GMPR);
  if (!Changed)
    return PreservedAnalyses::all();
  PreservedAnalyses PA;
  // TODO: add here what we *do* preserve.
  return PA;
}

#undef DEBUG_TYPE
