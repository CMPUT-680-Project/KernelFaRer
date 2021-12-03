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

namespace StencilFaRer {

// Replaces the corresponding basic blocks of MatMul IR with a call to
// llvm.matrix.multiply.
bool runImpl(Function &F, StencilMatcher::Result &SPR) {
  // Have we changed in this function?
  bool Changed = false;
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
  StencilFaRer::StencilMatcher::Result SPR =
      StencilFaRer::StencilMatcher::run(F, LI, DT, SE);
  bool Changed = runImpl(F, SPR);
  if (!Changed)
    return PreservedAnalyses::all();
  PreservedAnalyses PA;
  // TODO: add here what we *do* preserve.
  return PA;
}

#undef DEBUG_TYPE
