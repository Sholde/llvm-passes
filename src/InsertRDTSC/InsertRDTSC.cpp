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

      // Get RDTSC
      FunctionType *RDTSCTy = FunctionType::get(IntegerType::getInt64Ty(CTX),
                                                /*IsVarArgs=*/false);

      FunctionCallee RDTSC = M.getOrInsertFunction("rdtsc", RDTSCTy);

      // Set attributes as per inferLibFuncAttributes in BuildLibCalls.cpp
      Function *RDTSCF = dyn_cast<Function>(RDTSC.getCallee());
      RDTSCF->setDoesNotThrow();

      // STEP 2: Inject global variables
      // -------------------------------

      // STEP 2.1: Inject a global variable that will hold the printf format
      // string for the count of cycle of each function
      // -------------------------------------------------------------------
      llvm::Constant *PrintfFormatStr =
        llvm::ConstantDataArray::getString(CTX,
                                           "==insert-rdtsc== %32s  %16ld cycles\n");

      Constant *PrintfFormatStrVar =
        M.getOrInsertGlobal("PrintfFormatStr", PrintfFormatStr->getType());
      dyn_cast<GlobalVariable>(PrintfFormatStrVar)->setInitializer(PrintfFormatStr);

      // STEP 2.2: Inject a global variable that will hold the printf format
      // string that show information about passe
      // -------------------------------------------------------------------
      llvm::Constant *MainPrintfFormatStr =
        llvm::ConstantDataArray::getString(CTX,
          "============================== Insert RDTSC passe ==============================\n"
          "==insert-rdtsc==\n"
          "==insert-rdtsc==                    function name         number of cycles\n"
          "==insert-rdtsc==\n");

      Constant *MainPrintfFormatStrVar =
        M.getOrInsertGlobal("MainPrintfFormatStr", MainPrintfFormatStr->getType());
      dyn_cast<GlobalVariable>(MainPrintfFormatStrVar)->setInitializer(MainPrintfFormatStr);

      // Handle global variable of each function
      std::map<llvm::StringRef, llvm::Value *> map_str_value;

      // Constant Definitions
      llvm::Constant *zero =
        llvm::ConstantInt::get(IntegerType::getInt64Ty(CTX),
                               0/*value*/,
                               true);

      // STEP 3: Inject global variables that will hold each function counter
      // --------------------------------------------------------------------
      for (auto &F : M) {
        if (F.isDeclaration())
          continue;

        auto func_str = F.getName();

        // Global Variable Declarations
        GlobalVariable* gvar_count = new GlobalVariable(
          /*Module=*/M,
          /*Type=*/IntegerType::getInt64Ty(CTX),
          /*isConstant=*/false,
          /*Linkage=*/GlobalValue::CommonLinkage,
          /*Initializer=*/0, // has initializer, specified below
          /*Name=*/"count");

        // Global Variable Definitions
        gvar_count->setInitializer(zero);

        // Update map
        map_str_value.insert(
          std::pair<llvm::StringRef, llvm::Value *>(func_str, gvar_count));
      }

      // STEP 4: For each function in the module, start by call rdtsc function,
      // and at its end call a second time to get elapsed time and update the
      // gloabal variable with atomic operation (fetch and add - to provide
      // thread safety)
      // ----------------------------------------------------------------------
      for (auto &F : M) {
        if (F.isDeclaration())
          continue;

        // Get an IR builder. Sets the insertion point to the top of the function
        IRBuilder<> BuilderBeg(&*F.getEntryBlock().getFirstInsertionPt());

        // Inject a global variable that contains the function name
        //auto FuncName = BuilderBeg.CreateGlobalStringPtr(F.getName());

        // Printf requires i8*, but PrintfFormatStrVar is an array: [n x i8]. Add
        // a cast: [n x i8] -> i8*
        llvm::Value *FormatStrPtr =
          BuilderBeg.CreatePointerCast(PrintfFormatStrVar, PrintfArgTy, "formatStr");

        // The following is visible only if you pass -debug on the command line
        // *and* you have an assert build.
        LLVM_DEBUG(dbgs() << " Injecting call to printf inside " << F.getName()
                   << "\n");

        // Store rdtsc value
        llvm::Value *beg = NULL;
        llvm::Value *end = NULL;

        // Adding at the begin of the function
        if (F.getName() != RDTSCF->getName())
          {
            // Call rdtsc to get the clock start
            beg = BuilderBeg.CreateCall(RDTSC);
          }

        // Get the name of the current function
        auto func_str = F.getName();

        // Adding at the end of the function
        for (auto &BB: F)
          {
            for (auto &II: BB)
              {
                Instruction *I = &II;
                if (ReturnInst *RI = dyn_cast<ReturnInst>(I))
                  {
                    // Get an IR builder. Sets the insertion point to the top of the function
                    IRBuilder<> BuilderEnd(RI);

                    if (func_str != RDTSCF->getName())
                      {
                        // Call rdtsc to get the clock stop
                        end = BuilderEnd.CreateCall(RDTSC);

                        // Call sub to get elpased time
                        BinaryOperator *op_sub =
                          BinaryOperator::CreateSub(end, beg, "end-beg", RI);

                        // Atomic fetch and add
                        AtomicRMWInst *armwi =
                          new AtomicRMWInst(
                             /* BinOp */ llvm::AtomicRMWInst::Add,
                             /* ptr */ map_str_value[func_str],
                             /* val */ op_sub,
                             /* alignement */ Align(),
                             /* ordereing */ llvm::AtomicOrdering::SequentiallyConsistent,
                             /* SyncScope */ SyncScope::System,
                             /* insert before */ RI);
                      }

                    // STEP 5: Print the counter of each function
                    // ------------------------------------------
                    if (func_str == "main")
                      {
                        //
                        llvm::Value *MainFormatStrPtr =
                          BuilderBeg.CreatePointerCast(MainPrintfFormatStrVar,
                                                       PrintfArgTy,
                                                       "formatStr");

                        // Main message
                        BuilderEnd.CreateCall(Printf, {MainFormatStrPtr});

                        // Printing
                        for (auto &p: map_str_value)
                          {
                            // Get name of the function
                            auto func_str = p.first;

                            if (func_str != "rdtsc")
                              {
                                // Inject a global variable that contains the
                                // function name
                                auto FuncName =
                                  BuilderBeg.CreateGlobalStringPtr(func_str);

                                // Load global variable
                                LoadInst *gvar =
                                  new LoadInst(IntegerType::getInt64Ty(CTX),
                                               p.second,
                                               "load_gvar",
                                               RI);

                                // Finally, inject a call to printf
                                BuilderEnd.CreateCall(Printf,
                                                      {FormatStrPtr,
                                                       FuncName,
                                                       gvar});
                              }
                          }
                      }
                  }
              }
          }

        this->count_insertion++;
      }

      // If we modify the IR, then we need to specify with TRUE
      if (this->count_insertion != 0)
        return true;

      //
      return false;
    }
  };
}

//
char InsertRDTSC::ID = 0;
static RegisterPass<InsertRDTSC> X("insert-rdtsc", "Insert RDTSC Pass");
static RegisterStandardPasses Y(PassManagerBuilder::EP_EarlyAsPossible,
                                [](const PassManagerBuilder &Builder,
                                   legacy::PassManagerBase &PM) { PM.add(new InsertRDTSC()); });
