//===- CountBranch.cpp - Example code from "Writing an LLVM Pass" ---------------===//
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
#include "llvm/IR/Instruction.h"
#include "llvm/IR/Instructions.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

#define DEBUG_TYPE "count-branch"

namespace {
  // CountBranch - The first implementation
  struct CountBranch : public FunctionPass {
    static char ID; // Pass identification, replacement for typeid
    static uint64_t global_counter;
    uint64_t local_counter = 0;

    CountBranch() : FunctionPass(ID) {}

    bool runOnFunction(Function &F) override {
      // Function
#ifdef DEBUG
      errs() << "Visiting function: " << F.getName() << "\n";
#endif

      // Basic block
      for (BasicBlock &BB : F) {
#ifdef DEBUG
        errs() << "Visiting BB: " << BB << "\n";
#endif

        // Instruction
        for (Instruction &II : BB) {
#ifdef DEBUG
          errs() << "Visiting II: " << II  << "\n";
#endif

          // work
          Instruction *I = &II;
          if (BranchInst *BI = dyn_cast<BranchInst>(I)) {
            errs() << "Encountered a branch instruction " << BB << "\n";
            ++local_counter;
          }
        }
      }

      global_counter += local_counter;
      errs() << "Function: " << F.getName() << " have " << local_counter << " branch instruction" << "\n";

      return false;
    }

    bool doFinalization(Module &M) override {
      errs() << "Global Branch Counter: " << global_counter << '\n';
      return false;
    }
  };
}

char CountBranch::ID = 0;
uint64_t CountBranch::global_counter = 0;
static RegisterPass<CountBranch> X("count-branch", "Count Branch Pass");
