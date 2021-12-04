//===- GEMMFaRer.h - Matrix-Multiply Replacer Pass --------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This pass implements an idiom recognizer that transforms matrix-multiply
// loops into a call to llvm.matrix.multiply.* intrinsic. In cases that this
// kicks in, it can be a significant performance win.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_TRANSFORMS_SCALAR_STENCILFARER_H
#define LLVM_TRANSFORMS_SCALAR_STENCILFARER_H

#include "llvm/ADT/SetVector.h"
#include "llvm/Analysis/LoopInfo.h"
#include "llvm/Analysis/ScalarEvolution.h"
#include "llvm/IR/Dominators.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/PassManager.h"
#include "llvm/IR/Value.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

namespace StencilFaRer {

class StencilComputation {
  Loop &L;
  Instruction &ReductionStore;

public:
  StencilComputation(Loop &L, Instruction &RS) : L(L), ReductionStore(RS) {}
  Loop &getAssociatedLoop() const { return L; }
  Instruction &getReductionStore() const { return ReductionStore; }
};

/// Performs Matrix-Multiply Recognition Pass.
struct StencilMatcher {
public:
  using Result =
      std::unique_ptr<SmallVector<std::unique_ptr<StencilComputation>, 4>>;
  static Result run(Function &F, LoopInfo &LI, DominatorTree &DT,
                    ScalarEvolution &SE);
};

} // namespace StencilFaRer

namespace llvm {
/// Performs Matrix-Multiply Replacer Pass.
struct StencilFinderPass : public PassInfoMixin<StencilFinderPass> {
  friend PassInfoMixin<StencilFinderPass>;

public:
  static PreservedAnalyses run(Function &F, FunctionAnalysisManager &FAM);
};

} // end of namespace llvm

#endif /* LLVM_TRANSFORMS_SCALAR_STENCILFARER_H */
