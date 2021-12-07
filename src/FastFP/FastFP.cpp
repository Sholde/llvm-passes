//===- FastFP.cpp - Example code from "Writing an LLVM Pass" --------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file implements two versions of the LLVM "Hello World" pass described
// in docs/WritingAnLLVMPass.html
//
//===----------------------------------------------------------------------===//

#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h"
#include "llvm/Transforms/Utils/BasicBlockUtils.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

#define DEBUG_TYPE "fast-fp"

namespace {
  // FastFP - The first implementation
  struct FastFP : public FunctionPass {
    static char ID; // Pass identification, replacement for typeid

    FastFP() : FunctionPass(ID) {}

    bool runOnFunction(Function &F) override {
      // Basic block
      for (BasicBlock &BB : F) {
        // Instruction
        for (Instruction &II : BB) {
          // Work - Replace all double precision to simple precision
          Instruction *I = &II;

          if (AllocaInst *AI = dyn_cast<AllocaInst>(I)) {
            if (AI->getAllocatedType()->getTypeID() == Type::DoubleTyID) {
              errs() << "Encountered a double precision allocation\n";

              //auto *old_target = AI;
              //LLVMContext &context = F.getContext();
              //Type *type = Type::getFloatTy(context);
              //old_target->mutateType(old_target->getType()->getWithNewBitWidth(32));

              //auto *old_target = AI;
              //Value *d = old_target->getOperand(0);
              //errs() << d->getParent()->getGlobalValueSummary();
              //LLVMContext &context = F.getContext();
              //Type *type = Type::getFloatTy(context);
              //Value *f = UndefValue::get(type);
              //d->replaceAllUsesWith(f);

              //auto *old_target = AI;
              //
              ////get the type of long double and construct new AllocaInst
              //LLVMContext &context = F.getContext();
              //Type *type = Type::getFloatTy(context);
              //AllocaInst *new_target = new AllocaInst(type, 0, "newfloat", old_target);
              //
              ////new_target->takeName(old_target);
              ////transform(old_target, new_target, type);
              ////old_target->eraseFromParent();
              //
              ////auto *nf = new AllocaInst(Type::getFloatTy(F.getContext()), 0, "new float");
              ////BasicBlock::iterator ii(AI);
              ////ReplaceInstWithInst(old_target->getParent()->getInstList(), ii, new_target);

              //errs() << BB;
              //errs() << old_target->getOperand(0)->getValueID();
            }
          }
          else if (BinaryOperator *BO = dyn_cast<BinaryOperator>(I)) {
            if (BO->getOperand(0)->getType()->getTypeID() == Type::DoubleTyID) {
              //
              errs() << "Encountered a double precision binary operator\n";

              // Get instruction
              auto *old_target = BO;

              // Get operands
              Value *a = old_target->getOperand(0);
              Value *b = old_target->getOperand(1);

              // Cast operands to float
              CastInst *float_a =
                new FPTruncInst(a, Type::getFloatTy(F.getContext()), "d2f_a", old_target);
              CastInst *float_b =
                new FPTruncInst(b, Type::getFloatTy(F.getContext()), "d2f_b", old_target);

              // Make operation on cast operands
              BinaryOperator *op =
                BinaryOperator::Create(old_target->getOpcode(), float_a,
                                       float_b, "op_a_b", old_target);

              // Get lvalue of this op
              Value *c = llvm::cast<Value>(op);

              // Cast this lvalue to double
              CastInst *double_c =
                new FPExtInst(c, Type::getDoubleTy(F.getContext()), "f2d_c", old_target);

              // Get lvalue of old target instruction and replace it by my result
              Value *d = llvm::cast<Value>(old_target);
              d->replaceAllUsesWith(double_c);

              //
              //old_target->eraseFromParent();
            }
          }
//          else if (LoadInst *LI = dyn_cast<LoadInst>(I)) {
//            if (LI->getOperand(0)->getType()->getArrayElementType()->getTypeID() == Type::DoubleTyID) {
//              errs() << "Encountered a double precision load\n";
//              auto *old_target = LI;
//
//              Value *a = old_target->getOperand(0);
//              Type *type = Type::getFloatTy(F.getContext());
//
//              CastInst *float_a =
//                new FPTruncInst(a, type, "conv", old_target);
//
//              CastInst *ptr_float_a =
//                new FPTruncInst(float_a, Type::getFloatPtrTy(F.getContext()), "conv", old_target);
//
//              LoadInst *load = new LoadInst(type, ptr_float_a, "load", old_target);
//
//              this->find = true;
//              this->B = &BB;
//            }
//          }
        }
      }

      return true;
    }

    bool doFinalization(Module &M) override {
      return false;
    }
  };
}

char FastFP::ID = 0;
static RegisterPass<FastFP> X("fast-fp", "Fast Floating Point Pass");
