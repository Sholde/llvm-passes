//===- InsertRDTSC.cpp - Example code from "Writing an LLVM Pass" ---------===//
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

// ref: https://github.com/banach-space/llvm-tutor/blob/main/lib/InjectFuncCall.cpp

#include "llvm/Pass.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/Instructions.h"
#include "llvm/Support/raw_ostream.h"

#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"

using namespace llvm;

#define DEBUG_TYPE "insert-rdtsc"

namespace {
  // InsertRDTSC - The first implementation
  struct InsertRDTSC : public ModulePass {
    static char ID; // Pass identification, replacement for typeid
    uint64_t count_insertion = 0;

    InsertRDTSC() : ModulePass(ID) {}

    bool runOnModule(Module &M) override {

      auto &CTX = M.getContext();
      PointerType *PrintfArgTy = PointerType::getUnqual(Type::getInt8Ty(CTX));

      // STEP 1: Inject the declaration of printf
      // ----------------------------------------
      // Create (or _get_ in cases where it's already available) the following
      // declaration in the IR module:
      //    declare i32 @printf(i8*, ...)
      // It corresponds to the following C declaration:
      //    int printf(char *, ...)
      FunctionType *PrintfTy = FunctionType::get(IntegerType::getInt32Ty(CTX),
                                                 PrintfArgTy,
                                                 /*IsVarArgs=*/true);

      FunctionCallee Printf = M.getOrInsertFunction("printf", PrintfTy);

      // Set attributes as per inferLibFuncAttributes in BuildLibCalls.cpp
      Function *PrintfF = dyn_cast<Function>(Printf.getCallee());
      PrintfF->setDoesNotThrow();
      PrintfF->addParamAttr(0, Attribute::NoCapture);
      PrintfF->addParamAttr(0, Attribute::ReadOnly);

      FunctionType *RDTSCTy = FunctionType::get(IntegerType::getInt64Ty(CTX),
                                                /*IsVarArgs=*/true);

      FunctionCallee RDTSC = M.getOrInsertFunction("rdtsc", RDTSCTy);

      // Set attributes as per inferLibFuncAttributes in BuildLibCalls.cpp
      Function *RDTSCF = dyn_cast<Function>(RDTSC.getCallee());
      //RDTSCF->setDoesNotThrow();
      //RDTSCF->addParamAttr(0, Attribute::NoCapture);
      //RDTSCF->addParamAttr(0, Attribute::ReadOnly);

      // STEP 2: Inject a global variable that will hold the printf format string
      // ------------------------------------------------------------------------
      llvm::Constant *PrintfFormatStr =
        llvm::ConstantDataArray::getString(CTX,
                                           "==insert-rdtsc== Hello from: %s\n"
                                           "==insert-rdtsc==   cycles: %ld\n");

      Constant *PrintfFormatStrVar =
        M.getOrInsertGlobal("PrintfFormatStr", PrintfFormatStr->getType());
      dyn_cast<GlobalVariable>(PrintfFormatStrVar)->setInitializer(PrintfFormatStr);

      // STEP 3: For each function in the module, inject a call to printf
      // ----------------------------------------------------------------
      for (auto &F : M) {
        if (F.isDeclaration())
          continue;

        // Get an IR builder. Sets the insertion point to the top of the function
        IRBuilder<> Builder(&*F.getEntryBlock().getFirstInsertionPt());

        // Inject a global variable that contains the function name
        auto FuncName = Builder.CreateGlobalStringPtr(F.getName());

        // Printf requires i8*, but PrintfFormatStrVar is an array: [n x i8]. Add
        // a cast: [n x i8] -> i8*
        llvm::Value *FormatStrPtr =
          Builder.CreatePointerCast(PrintfFormatStrVar, PrintfArgTy, "formatStr");

        // The following is visible only if you pass -debug on the command line
        // *and* you have an assert build.
        LLVM_DEBUG(dbgs() << " Injecting call to printf inside " << F.getName()
                   << "\n");

        llvm::Value *ret = Builder.CreateCall(RDTSC);

        // Finally, inject a call to printf
        Builder.CreateCall(Printf,
                           {FormatStrPtr, FuncName, Builder.getInt64(4)});

        this->count_insertion++;
      }

      if (this->count_insertion != 0)
        return true;

      return false;
    }
  };
}

char InsertRDTSC::ID = 0;
static RegisterPass<InsertRDTSC> X("insert-rdtsc", "Insert RDTSC Pass");
static RegisterStandardPasses Y(PassManagerBuilder::EP_EarlyAsPossible,
                                [](const PassManagerBuilder &Builder,
                                   legacy::PassManagerBase &PM) { PM.add(new InsertRDTSC()); });
