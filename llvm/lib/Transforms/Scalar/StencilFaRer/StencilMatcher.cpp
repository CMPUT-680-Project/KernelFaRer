//===- StencilMatcher.cpp - Stencil Recognition Pass ----------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "llvm/ADT/SmallSet.h"
#include "llvm/Analysis/IVDescriptors.h"
#include "llvm/Analysis/ScalarEvolution.h"
#include "llvm/Analysis/ScalarEvolutionExpressions.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/InstrTypes.h"
#include "llvm/IR/PatternMatch.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Transforms/Scalar/StencilFaRer.h"

#include <algorithm>
#include <iterator>
#include <vector>

using namespace llvm;
using namespace llvm::PatternMatch;
using namespace StencilFaRer;

#define DEBUG_TYPE "stencil-matcher"

namespace llvm {
// Below novel and extended versions of matchers from PatternMatch are provided.
namespace PatternMatch {

// Provide a nonconst and const version as in PatternMatch.h.
/// Match a phi, capturing it if we match.
inline PatternMatch::bind_ty<PHINode> m_PHI(PHINode *&PHI) { return PHI; }
inline PatternMatch::bind_ty<const PHINode> m_PHI(const PHINode *&PHI) {
  return PHI;
}

/// Class that matches a phi's 1st and 2nd incoming values.
template <typename InVal1Ty, typename InVal2Ty> struct PHI_match {
  InVal1Ty InVal1;
  InVal2Ty InVal2;

  PHI_match(const InVal1Ty &IV1, const InVal2Ty &IV2)
      : InVal1(IV1), InVal2(IV2) {}

  template <typename OpTy> bool match(OpTy *V) {
    if (auto *PHI = dyn_cast<PHINode>(V)) {
      auto N = PHI->getNumOperands();
      if (N != 2)
        return false;
      return InVal1.match(PHI->getIncomingValue(0)) &&
             InVal2.match(PHI->getIncomingValue(1));
    }
    return false;
  }
};

template <typename InVal1Ty, typename InVal2Ty>
inline PHI_match<InVal1Ty, InVal2Ty> m_PHI(InVal1Ty InVal1, InVal2Ty InVal2) {
  return PHI_match<InVal1Ty, InVal2Ty>(InVal1, InVal2);
}

// Match a GEP, capturing it if we match.
inline PatternMatch::bind_ty<GetElementPtrInst> m_GEP(GetElementPtrInst *&GEP) {
  return GEP;
}
inline PatternMatch::bind_ty<const GetElementPtrInst>
m_GEP(const GetElementPtrInst *&GEP) {
  return GEP;
}

// Base classs for m_OneOf matcher.
template <typename... List> struct match_one_of {
  // Empty instance should never be used.
  match_one_of() = delete;
};

// Empty list is never a match.
template <> struct match_one_of<> {
  template <typename ITy> bool match(ITy *V) { return false; }
};

// Matches either the head or tail of variadic OneOf argument list.
template <typename Head, typename... List> struct match_one_of<Head, List...> {
  Head Op;
  match_one_of<List...> Next;

  match_one_of(Head Op, List... Next) : Op(Op), Next(Next...) {}

  template <typename ITy> bool match(ITy *V) {
    return Op.match(V) || Next.match(V);
  }
};

// Base classs for m_OneOf matcher.
template <typename... List> struct match_one_of_trackable {
  // Empty instance should never be used.
  match_one_of_trackable(int &Index) = delete;
};

// Empty list is never a match.
template <> struct match_one_of_trackable<> {
  int Index;
  match_one_of_trackable(int &Index) : Index(Index) {}
  template <typename ITy> bool match(ITy *V) { return false; }
};

// Matches either the head or tail of variadic OneOf argument list.
template <typename Head, typename... List>
struct match_one_of_trackable<Head, List...> {
  int &Index;
  Head Op;
  match_one_of_trackable<List...> Next;

  match_one_of_trackable(int &Index, Head Op, List... Next)
      : Index(Index), Op(Op), Next(Index, Next...) {}

  template <typename ITy> bool match(ITy *V) {
    if (Op.match(V)) {
      return true;
    } else {
      Index++;
      if (Next.match(V)) {
        return true;
      } else {
        Index = -1;
        return false;
      }
    }
  }
};

/// This helper class is used to or-combine a list of matchers.
/// Matches one of the patterns in a list.
template <typename... PatternList>
inline match_one_of<PatternList...> m_OneOf(PatternList... Patterns) {
  return match_one_of<PatternList...>(Patterns...);
}

/// This helper class is used to or-combine a list of matchers.
/// Matches one of the patterns in a list.
template <typename... PatternList>
inline match_one_of_trackable<PatternList...>
m_OneOfTrackable(int &Index, PatternList... Patterns) {
  Index = 0;
  return match_one_of_trackable<PatternList...>(Index, Patterns...);
}

/// This helper class implements the same behavior as m_CombineOr but also
/// accepts a list of Value* that must be reset (set to nullptr) in case Op1
/// does not match. This behabior is usefull for matchers that bind an optional
/// value.
template <typename LHSTy, typename RHSTy, typename... ValuesTy>
struct CombineOrWithReset_match {
  LHSTy L;
  RHSTy R;
  SmallVector<Value **, 4> Values;

  CombineOrWithReset_match(LHSTy L, RHSTy R, ValuesTy... Values)
      : L(L), R(R), Values({Values...}) {}

  template <typename ITy> bool match(ITy *V) {
    if (L.match(V))
      return true;
    for (Value **ResetV : Values)
      if (ResetV != nullptr)
        (*ResetV) = nullptr;
    return R.match(V);
  }
};

} // End namespace PatternMatch
} // End namespace llvm

// A helper function that returns the outermost PHINode (induction variable)
// associated with V. The outermost induction variable has two incomming
// values, a initialization value (ConstantInt) and a post-increment value
// (AddInst). A chain of PHINodes is the IR pattern produced when compiling
// tiled loop nests.
PHINode *extractOutermostPHI(PHINode *const &V) {
  if (!isa<PHINode>(V))
    return nullptr;

  SmallSetVector<const PHINode *, 8> WorkQueue;
  WorkQueue.insert(V);

  while (!WorkQueue.empty()) {
    const auto *PHI = WorkQueue.front();
    WorkQueue.remove(PHI);

    if (match(PHI,
              m_OneOf(m_PHI(m_c_Add(m_Specific(PHI), m_Value()), m_Value()),
                      m_PHI(m_Value(), m_c_Add(m_Specific(PHI), m_Value())),
                      m_PHI(m_ConstantInt(), m_ConstantInt())))) {
      return const_cast<PHINode *>(PHI);
    }

    for (const Use &Op : PHI->incoming_values())
      if (auto *InPHI = dyn_cast_or_null<PHINode>(&Op))
        WorkQueue.insert(InPHI);
  }
  return nullptr;
}

// A helper function that the inserts in Loops list the innermost loop nested
// in L, or L itself if L does not have sub-loops.
static void collectInnermostLoops(const Loop *L,
                                  SmallSetVector<const Loop *, 8> &Loops) {
  SmallSetVector<const Loop *, 8> WorkQueue;

  if (L->getSubLoops().size() == 0) {
    Loops.insert(L);
    return;
  }

  for (const auto *SL : L->getSubLoops())
    WorkQueue.insert(SL);

  while (!WorkQueue.empty()) {
    const auto *SL = WorkQueue.front();
    WorkQueue.remove(SL);

    bool HasSubLoop = false;
    for (const auto *SSL : SL->getSubLoops()) {
      HasSubLoop = true;
      WorkQueue.insert(SSL);
    }
    if (!HasSubLoop)
      Loops.insert(SL);
  }
}

// A helper function that collects into Loops all loops in a function that are
// nested at level 1 or deeper
static void
collectLoopsWithDepthOneOrDeeper(LoopInfo &LI,
                                 SmallSetVector<const Loop *, 8> &Loops) {
  for (auto *L : LI.getLoopsInPreorder()) {
    if (Loops.count(L) == 0) {
      collectInnermostLoops(L, Loops);
    }
  }
}

// A helper function that tries to find the upper bound (UBound) of a loop
// associated with the induction variable (IndVar). If the upper bound is found
// and is a constant, then this function returns true and sets the incoming
// argument UBound to whichever value the loop associated with IndVar has.
// Otherwise, this function returns false and sets UBound to nullptr.
// If the incoming PHINode is a Phi(true, false) as in tripcount == 2 loops,
// the bound is not matched.
static bool matchLoopUpperBound(LoopInfo &LI, PHINode *IndVar, Value *&UBound) {
  BasicBlock *Header = IndVar->getParent();
  Loop *L = LI.getLoopFor(Header);
  if (L == nullptr)
    return false;
  SmallVector<BasicBlock *, 4> LoopExitingBBs;
  L->getExitingBlocks(LoopExitingBBs);

  // Iterate over branch instructions
  for (auto *BB : LoopExitingBBs) {
    auto *Term = BB->getTerminator();
    if (auto *BR = dyn_cast<BranchInst>(Term)) {
      // Pick the comparison instruction for this loop header
      for (auto *SuccBB : BR->successors()) {
        if (SuccBB != Header)
          continue;
        // For loops with trip_count == 2, Combine redundant instructions
        // replaces the integer induction variable with a phi(true, false)
        if (auto *Phi = dyn_cast<PHINode>(BR->getCondition()))
          if (match(Phi, m_CombineOr(m_PHI(m_Zero(), m_One()),
                                     m_PHI(m_One(), m_Zero())))) {
            IRBuilder<> IR(&Header->getParent()->getEntryBlock());
            UBound = IR.getInt64(2);
            return true;
          }
        if (auto *CMP = dyn_cast<ICmpInst>(BR->getCondition())) {
          ICmpInst::Predicate Pred;
          // Iterate over header phis, find the one that matches the upper
          // bound
          for (BasicBlock::const_iterator I = Header->begin(); isa<PHINode>(I);
               I++) {
            const auto *PHI = static_cast<const Value *>(&*I);
            if (!match(CMP,
                       m_c_ICmp(
                           Pred,
                           m_CombineOr(m_Specific(PHI),
                                       m_c_Add(m_Specific(PHI), m_Value())),
                           m_OneOf(m_ZExt(m_Value(UBound)),
                                   m_SExt(m_Value(UBound)), m_Value(UBound)))))
              continue;
            if (isa<PHINode>(UBound)) {
              // GEMM loops are not triangular.
              UBound = nullptr;
              return false;
            }
            return true;
          }
        }
      }
    }
  }
  return false;
}

// A helper function that tries to find the lower bound (LBound) of a loop
// associated with the induction variable (IndVar). If the lower bound is found,
// then this function returns true and sets the incoming argument LBound to
// whichever value the loop associated with IndVar has. Otherwise, this function
// returns false and sets LBound to nullptr.
static bool matchLoopLowerBound(LoopInfo &LI, PHINode *IndVar, Value *&LBound) {
  auto LBoundMatcher = m_OneOf(m_ZExt(m_Value(LBound)), m_SExt(m_Value(LBound)),
                               m_Value(LBound));
  if (match(IndVar, m_OneOf(m_PHI(m_c_Add(m_Specific(IndVar), m_Value()),
                                  LBoundMatcher),
                            m_PHI(LBoundMatcher,
                                  m_c_Add(m_Specific(IndVar), m_Value()))))) {
    return true;
  } else {
    LBound = nullptr;
    return false;
  }
}

// A helper function that returns the outermost loop associated with one of
// the incoming induction variables in IVar.
static Loop *getOuterLoop(LoopInfo &LI, const SmallVector<Value *, 3> &IVars) {
  Loop *outer = nullptr;
  unsigned int min_depth = UINT_MAX;
  for (auto *iv : IVars) {
    Loop *l = LI.getLoopFor(static_cast<const PHINode *>(iv)->getParent());
    unsigned int depth = l->getLoopDepth();
    if (depth < min_depth) {
      min_depth = depth;
      outer = l;
    }
  }
  return outer;
}

typedef std::tuple<int, PHINode *, uint64_t>
    ScaledPHINode; // <ScaleOp, PHINode *, scale>

const int SCALEOP_NONE = 0;
// const int SCALEOP_MUL = 1;
// const int SCALEOP_SHL = 2;
inline auto scaledPHIOrPHI(int &ScaleOp, PHINode *&PHI, uint64_t &scale) {
  return m_OneOfTrackable(ScaleOp, m_PHI(PHI),
                          m_c_Mul(m_PHI(PHI), m_ConstantInt(scale)),
                          m_Shl(m_PHI(PHI), m_ConstantInt(scale)));
}

const int OFFSETOP_NONE = 0;
const int OFFSETOP_ADD = 1;
const int OFFSETOP_OR1 = 2;
inline auto linearFuncOfPHI(int &ScaleOp, int &OffsetOp, PHINode *&PHI,
                            uint64_t &scale, uint64_t &offset) {
  return m_OneOfTrackable(
      OffsetOp, scaledPHIOrPHI(ScaleOp, PHI, scale),
      m_c_Add(scaledPHIOrPHI(ScaleOp, PHI, scale), m_ConstantInt(offset)),
      m_c_Or(scaledPHIOrPHI(ScaleOp, PHI, scale), m_SpecificInt(1)));
}

inline bool extractBasePtrOpAndPHIs(GetElementPtrInst *PtrOp, Value *&BasePtrOp,
                                    SmallVector<ScaledPHINode, 3> &PHIs,
                                    SmallVector<uint64_t, 3> &offsets) {
  do {
    auto N = PtrOp->getNumOperands();
    for (size_t i = 1; i < N; ++i) {
      PHINode *PHI = nullptr;
      int which = -1;
      int OffsetOp;
      int ScaleOp;
      uint64_t scale;
      uint64_t offset;
      if (match(PtrOp->getOperand(N - i),
                m_OneOfTrackable(
                    which,
                    linearFuncOfPHI(ScaleOp, OffsetOp, PHI, scale, offset),
                    m_ConstantInt()))) {
        if (which == 0) { // linearFuncOfPHI

          if (ScaleOp == SCALEOP_NONE)
            scale = 1;
          PHIs.push_back(std::make_tuple(ScaleOp, PHI, scale));

          switch (OffsetOp) {
          case OFFSETOP_NONE:
            offsets.push_back(0);
            break;
          case OFFSETOP_ADD:
            offsets.push_back(offset);
            break;
          case OFFSETOP_OR1:
            offsets.push_back(1);
            break;
          default:
            dbgs() << "! Unknown OffsetOp. This should not happen.\n";
            return false;
          }

        } else if (which == 1) { // m_ConstantInt
          break;
        } else {
          dbgs() << "! Unknown match result. This should not happen.\n";
          return false;
        }

      } else
        return false;
    }
  } while (match(PtrOp->getPointerOperand(),
                 m_CombineOr(m_Load(m_GEP(PtrOp)), m_GEP(PtrOp))));
  BasePtrOp = PtrOp;
  return true;
}

inline bool matchStencilLoad(const Value *Inst, Value *&BasePtrOp,
                             SmallVector<ScaledPHINode, 3> &PHIs,
                             SmallVector<uint64_t, 3> &offsets) {
  GetElementPtrInst *PtrOp;
  if (!match(Inst, m_Load(m_GEP(PtrOp))))
    return false;
  return extractBasePtrOpAndPHIs(PtrOp, BasePtrOp, PHIs, offsets);
}

static bool matchStencilStore(const Value *Inst, Value *&BasePtrOp,
                              SmallVector<ScaledPHINode, 3> &PHIs,
                              SmallVector<uint64_t, 3> &offsets,
                              Value *&ValueOp) {
  GetElementPtrInst *PtrOp;
  if (!match(Inst, m_Store(m_Value(ValueOp), m_GEP(PtrOp))))
    return false;
  return extractBasePtrOpAndPHIs(PtrOp, BasePtrOp, PHIs, offsets);
}

inline bool matchStencilExpr(const Value *seed, const Value *OutPtr,
                             const SmallVector<ScaledPHINode, 3> &ScaledPHIs,
                             SmallVector<Value *, 3> &InPtrs,
                             bool &SelfReferencing) {
  dbgs() << "[Expression Tree]\n";
  SmallSetVector<const Value *, 8> WorkQueue;
  WorkQueue.insert(seed);

  Value *BinLHS;
  Value *BinRHS;
  Value *UnArg;

  Value *LoadPtr;
  SmallVector<ScaledPHINode, 3> LoadPHIs;
  SmallVector<uint64_t, 3> LoadOffsets;
  SmallSet<Value *, 3> LoadPtrs;

  while (!WorkQueue.empty()) {
    const auto *v = WorkQueue.front();
    WorkQueue.remove(v);

    if (match(v, m_c_BinOp(m_Value(BinLHS), m_Value(BinRHS)))) {
      dbgs() << "BinOp: ";
      v->print(dbgs());
      dbgs() << "\n";
      WorkQueue.insert(BinLHS);
      WorkQueue.insert(BinRHS);

    } else if (match(v, m_UnOp(m_Value(UnArg)))) {
      dbgs() << "UnOp: ";
      v->print(dbgs());
      dbgs() << "\n";
      WorkQueue.insert(UnArg);

    } else if (match(v, m_Constant())) {
      dbgs() << "Constant (Leaf): ";
      v->print(dbgs());
      dbgs() << "\n";

    } else if (matchStencilLoad(v, LoadPtr, LoadPHIs, LoadOffsets)) {
      if (LoadPHIs.size() != ScaledPHIs.size()) {
        dbgs() << "Fail: Number of PHI Nodes differ between stencil load and store.\n";
        return false;
      }

      for (size_t i = 0; i < LoadPHIs.size(); ++i)
        if (LoadPHIs[i] != ScaledPHIs[i]) {
          dbgs() << "Fail: PHI node mismatch between stencil load and store indices.\n";
          return false;
        }

      // Record that the LoadPtr is an input for the stencil computation
      auto res = LoadPtrs.insert(LoadPtr);
      if (res.second)
        InPtrs.push_back(LoadPtr);

      // Set SelfReferencing flag if stencil references itself
      if (LoadPtr == OutPtr)
        SelfReferencing = true;

      dbgs() << "Stencil Load (Leaf) to array `" << LoadPtr->getName()
             << "` | Offsets: ";
      for (uint64_t offset : LoadOffsets) {
        dbgs() << int64_t(offset) << " ";
      }
      dbgs() << "\n";

      LoadPHIs.clear();
      LoadOffsets.clear();

    } else if (isa<LoadInst>(v)) {
      dbgs() << "Fail: Unrecognized load: ";
      v->print(dbgs());
      dbgs() << "\n";
      return false;

    } else {
      dbgs() << "Leaf: ";
      v->print(dbgs());
      dbgs() << "\n";
    }
  }

  if (InPtrs.empty()) {
    dbgs() << "Not a stencil expression; there are no input arrays.\n";
    return false;
  }

  dbgs() << "Found a stencil expr!\n";
  return true;
}

inline bool isPHIAuxIndVarForLoop(PHINode *phi, const Loop *L, LoopInfo &LI,
                                  ScalarEvolution &SE) {
  if (phi == nullptr) {
    dbgs() << "phi is nullptr. This should not happen\n";
    return false;
  }
  if (L->getLoopPreheader() == nullptr) {
    dbgs() << "Loop missing preheader. Trying to use loop info\n";
    if (LI.getLoopFor(phi->getParent()) == L) {
      dbgs() << "Matched loop via loop info\n";
      return true;
    }
    return false;
  }
  return L->isAuxiliaryInductionVariable(*phi, SE);
}

inline SmallSet<const Loop *, 4> getLoopVector(const Loop *L, size_t MinDepth) {
  SmallSet<const Loop *, 4> Loops;
  while (L != nullptr && L->getLoopDepth() > MinDepth) {
    Loops.insert(L);
    L = L->getParentLoop();
  }
  return Loops;
}

static bool matchStencil(Instruction &SeedInst, Value *&OutPtr,
                         SmallVector<Value *, 3> &IVars,
                         SmallVector<Value *, 3> &InPtrs, bool &SelfReferencing,
                         const Loop *L, LoopInfo &LI, ScalarEvolution &SE) {
  // Check for stencil store and extract induction variables
  Value *StoreInstAsValue = static_cast<Value *>(&SeedInst);
  SmallVector<ScaledPHINode, 3> ScaledPHIs;
  SmallVector<uint64_t, 3> StoreOffsets;
  Value *StoreValue;
  if (!matchStencilStore(StoreInstAsValue, OutPtr, ScaledPHIs, StoreOffsets,
                         StoreValue)) {
    dbgs() << "Not a stencil store.\n";
    return false;
  }

  dbgs() << "Potential stencil store to array `" << OutPtr->getName()
         << "` | Offsets: ";
  for (uint64_t offset : StoreOffsets) {
    dbgs() << int64_t(offset) << " ";
  }
  dbgs() << "\n";

  // Match PHIs to loops by checking if they are auxiliary induction vars
  SmallSet<const Loop *, 4> Loops =
      getLoopVector(L, L->getLoopDepth() - ScaledPHIs.size());
  for (size_t i = 0; i < ScaledPHIs.size(); ++i) {
    PHINode *PHI = std::get<1>(ScaledPHIs[i]);
    bool found = false;
    PHINode *outermostPHI = extractOutermostPHI(PHI);
    for (auto Loop : Loops) {
      if (isPHIAuxIndVarForLoop(outermostPHI, Loop, LI, SE)) {
        IVars.push_back(outermostPHI);
        Loops.erase(Loop);
        found = true;
        break;
      }
    }
    if (!found) {
      dbgs() << "Could not find a loop for PHI: ";
      std::get<1>(ScaledPHIs[i])->print(dbgs());
      return false;
    }
  }

  return matchStencilExpr(StoreValue, OutPtr, ScaledPHIs, InPtrs,
                          SelfReferencing);
}

namespace StencilFaRer {

StencilMatcher::Result StencilMatcher::run(Function &F, LoopInfo &LI,
                                           DominatorTree &DT,
                                           ScalarEvolution &SE) {
  auto ListOfStencils =
      std::make_unique<SmallVector<std::unique_ptr<StencilComputation>, 4>>();
  SmallSetVector<const Loop *, 8> LoopsToProcess;
  collectLoopsWithDepthOneOrDeeper(LI, LoopsToProcess);
  for (const auto *L : LoopsToProcess) {
    for (auto *BB : L->getBlocks()) {
      for (auto Inst = BB->begin(); Inst != BB->end(); Inst++) {
        if (!isa<StoreInst>(Inst))
          continue;

        Value *OutPtr;                  // Output array
        SmallVector<Value *, 3> IVars;  // Induction variables
        SmallVector<Value *, 3> InPtrs; // Input arrays
        bool SelfReferencing = false;

        if (matchStencil(*Inst, OutPtr, IVars, InPtrs, SelfReferencing, L, LI,
                         SE)) {
          bool matchBounds = true;
          for (auto *IVar : IVars) {
            Value *ILBound; // Induction variable lower bound
            Value *IUBound; // Induction variable upper bound
            if (matchLoopLowerBound(LI, static_cast<PHINode *>(IVar),
                                    ILBound) &&
                matchLoopUpperBound(LI, static_cast<PHINode *>(IVar),
                                    IUBound)) {
              InductionDescriptor IndDesc;
              const SCEV *PhiScev = SE.getSCEV(IVar);
              const SCEVAddRecExpr *AR = dyn_cast<SCEVAddRecExpr>(PhiScev);
              const SCEV *Step = AR->getStepRecurrence(SE);
              const SCEVConstant *ConstStep = dyn_cast<SCEVConstant>(Step);
              if (!ConstStep) {
                // TODO: Check for loop invariant step
                // !SE.isLoopInvariant(Step, LI.getLoopFor(phi->getParent()))
                dbgs() << "Loop step is not constant!\n";
              } else {
                dbgs() << "Loop step: ";
                ConstStep->getValue()->print(dbgs());
                dbgs() << "\n";
              }
              dbgs() << "Induction Variable ";
              IVar->print(dbgs());
              dbgs() << "\n";
              dbgs() << "Loop lower bound: ";
              ILBound->print(dbgs());
              dbgs() << "\n";
              dbgs() << "Loop upper bound: ";
              IUBound->print(dbgs());
              dbgs() << "\n";
            } else {
              matchBounds = false;
            }
          }
          if (matchBounds) {
            dbgs() << "Found a stencil!\n";
            if (SelfReferencing)
              dbgs() << "The stencil is self-referential.\n";
            dbgs() << "The stencil has " << InPtrs.size() << " input arrays.\n";
          }
        } else {
          continue;
        }

        Loop *OuterLoop = getOuterLoop(LI, IVars);
        // Verify that we only have one block we're exiting from.
        if (OuterLoop->getExitingBlock() == nullptr) {
          LLVM_DEBUG(dbgs() << "Loop had multiple exiting blocks.\n");
          continue;
        }

        // Verify that we only have one exit block to go to. We won't have
        // any way to determine how to get to multiple exits.
        if (OuterLoop->getExitBlock() == nullptr) {
          LLVM_DEBUG(dbgs() << "Loop had multiple exit blocks.\n");
          continue;
        }

        ListOfStencils->push_back(
            std::make_unique<StencilComputation>(*OuterLoop, *Inst));
      }
    }
  }

  dbgs() << "[Summary]\n";
  dbgs() << "Found " << ListOfStencils->size() << " stencil(s):\n";
  for (auto &SC : *ListOfStencils) {
    dbgs() << "Stencil loop starting at ";
    SC->getAssociatedLoop().getStartLoc().print(dbgs());
    dbgs() << " with reduction store at ";
    SC->getReductionStore().getDebugLoc().print(dbgs());
    dbgs() << "\n";
  }
  return ListOfStencils;
}

} // namespace StencilFaRer
