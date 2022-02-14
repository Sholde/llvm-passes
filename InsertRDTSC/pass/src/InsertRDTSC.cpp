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

    // Constructor
    InsertRDTSC() : ModulePass(ID) {}

    // Run pass
    bool runOnModule(Module &M) override {

      // Get context
      auto &CTX = M.getContext();

      // -------------------------
      // Step 1: Get argument type
      // -------------------------
      PointerType *PointerInt8Ty = PointerType::getUnqual(Type::getInt8Ty(CTX));
      PointerType *PointerInt64Ty =
        PointerType::getUnqual(Type::getInt64Ty(CTX));
      IntegerType *Int64Ty = IntegerType::getInt64Ty(CTX);
      Type *VoidTy = Type::getVoidTy(CTX);

      // ----------------------------------------------
      // Step 2: Injet the declaration of all functions
      // ----------------------------------------------

      /* Get rdtsc */
      FunctionType *rdtscTy = FunctionType::get(/*ReturnType=*/Int64Ty,
                                                /*IsVarArgs=*/false);

      FunctionCallee rdtsc = M.getOrInsertFunction("rdtsc", rdtscTy);

      // Set attributes
      Function *rdtscF = dyn_cast<Function>(rdtsc.getCallee());
      rdtscF->setDoesNotThrow();

      /* Get rdtscp */
      FunctionType *rdtscpTy =
        FunctionType::get(/*ReturnType=*/VoidTy,
                          /*ArgType=*/{PointerInt64Ty, PointerInt64Ty},
                          /*IsVarArgs=*/true);

      FunctionCallee rdtscp = M.getOrInsertFunction("rdtscp", rdtscpTy);

      // Set attributes
      Function *rdtscpF = dyn_cast<Function>(rdtscp.getCallee());
      rdtscpF->setDoesNotThrow();

      /* Get get_pid */
      FunctionType *get_pidTy = FunctionType::get(/*ReturnType=*/Int64Ty,
                                                  /*IsVarArgs=*/false);

      FunctionCallee get_pid = M.getOrInsertFunction("get_pid", get_pidTy);

      // Set attributes
      Function *get_pidF = dyn_cast<Function>(get_pid.getCallee());
      get_pidF->setDoesNotThrow();

      /* Get get_tid */
      FunctionType *get_tidTy = FunctionType::get(/*ReturnType=*/Int64Ty,
                                                  /*IsVarArgs=*/false);

      FunctionCallee get_tid = M.getOrInsertFunction("get_tid", get_tidTy);

      // Set attributes
      Function *get_tidF = dyn_cast<Function>(get_tid.getCallee());
      get_tidF->setDoesNotThrow();

      /* Get get_num_procs */
      FunctionType *get_num_procsTy = FunctionType::get(/*ReturnType=*/Int64Ty,
                                                        /*IsVarArgs=*/false);

      FunctionCallee get_num_procs =
        M.getOrInsertFunction("get_num_procs", get_num_procsTy);

      // Set attributes
      Function *get_num_procsF = dyn_cast<Function>(get_num_procs.getCallee());
      get_num_procsF->setDoesNotThrow();

      /* Get get_num_procs_available */
      FunctionType *get_num_procs_availableTy =
        FunctionType::get(/*ReturnType=*/Int64Ty,
                          /*IsVarArgs=*/false);

      FunctionCallee get_num_procs_available =
        M.getOrInsertFunction("get_num_procs_available",
                              get_num_procs_availableTy);

      // Set attributes
      Function *get_num_procs_availableF =
        dyn_cast<Function>(get_num_procs_available.getCallee());
      get_num_procs_availableF->setDoesNotThrow();

      /* Get insert_function */
      FunctionType *insert_functionTy =
        FunctionType::get(/*ReturnType=*/VoidTy,
                          /*ArgType=*/
                          {Int64Ty, Int64Ty, Int64Ty, Int64Ty,
                           Int64Ty, Int64Ty, PointerInt8Ty},
                          /*IsVarArgs=*/false);

      FunctionCallee insert_function =
        M.getOrInsertFunction("insert_function", insert_functionTy);

      // Set attributes
      Function *insert_functionF =
        dyn_cast<Function>(insert_function.getCallee());
      insert_functionF->setDoesNotThrow();

      /* Get write_function_info */
      FunctionType *write_function_infoTy =
        FunctionType::get(/*ReturnType=*/VoidTy,
                          /*ArgType=*/Int64Ty,
                          /*IsVarArgs=*/false);

      FunctionCallee write_function_info =
        M.getOrInsertFunction("write_function_info", write_function_infoTy);

      // Set attributes
      Function *write_function_infoF =
        dyn_cast<Function>(write_function_info.getCallee());
      write_function_infoF->setDoesNotThrow();

      /* Get write_function_info_in_csv_file */
      FunctionType *write_function_info_in_csv_fileTy =
        FunctionType::get(/*ReturnType=*/VoidTy,
                          /*ArgType=*/Int64Ty,
                          /*IsVarArgs=*/false);

      FunctionCallee write_function_info_in_csv_file =
        M.getOrInsertFunction("write_function_info_in_csv_file",
                              write_function_info_in_csv_fileTy);

      // Set attributes
      Function *write_function_info_in_csv_fileF =
        dyn_cast<Function>(write_function_info_in_csv_file.getCallee());
      write_function_info_in_csv_fileF->setDoesNotThrow();

      /* Get write_function_summary */
      FunctionType *write_function_summaryTy =
        FunctionType::get(/*ReturnType=*/VoidTy,
                          /*IsVarArgs=*/false);

      FunctionCallee write_function_summary =
        M.getOrInsertFunction("write_function_summary",
                              write_function_summaryTy);

      // Set attributes
      Function *write_function_summaryF =
        dyn_cast<Function>(write_function_summary.getCallee());
      write_function_summaryF->setDoesNotThrow();

      /* Get write_module_summary */
      FunctionType *write_module_summaryTy =
        FunctionType::get(/*ReturnType=*/VoidTy,
                          /*IsVarArgs=*/false);

      FunctionCallee write_module_summary =
        M.getOrInsertFunction("write_module_summary",
                              write_module_summaryTy);

      // Set attributes
      Function *write_module_summaryF =
        dyn_cast<Function>(write_module_summary.getCallee());
      write_module_summaryF->setDoesNotThrow();

      /* Get analyze_function */
      FunctionType *analyze_functionTy =
        FunctionType::get(/*ReturnType=*/VoidTy,
                          /*ArgType=*/Int64Ty,
                          /*IsVarArgs=*/false);

      FunctionCallee analyze_function =
        M.getOrInsertFunction("analyze_function", analyze_functionTy);

      // Set attributes
      Function *analyze_functionF =
        dyn_cast<Function>(analyze_function.getCallee());
      analyze_functionF->setDoesNotThrow();

      // -------------------------------------------
      // Step 3: Declare and initialize global index
      // -------------------------------------------

      llvm::Constant *zero =
        llvm::ConstantInt::get(/*Type=*/Int64Ty,
                               /*value*/0,
                               /*IsVarArgs=*/false);

      llvm::Constant *one =
        llvm::ConstantInt::get(/*Type=*/Int64Ty,
                               /*value*/1,
                               /*IsVarArgs=*/false);

      // Global Variable Declarations
      GlobalVariable* global_count_calls =
        new GlobalVariable(/*Module=*/M,
                           /*Type=*/Int64Ty,
                           /*isConstant=*/false,
                           /*Linkage=*/GlobalValue::CommonLinkage,
                           /*Initializer=*/0, // has initializer, specified below
                           /*Name=*/"global_count_calls");

      // Global Variable Definitions
      global_count_calls->setInitializer(zero);

      // ---------------------
      // Step 4: Insert  calls
      // ---------------------

      for (auto &F : M) {
        if (F.isDeclaration())
          continue;

        // Get the name of the current function
        auto func_str = F.getName();

        // -----------------------
        // Step 5: Insert at begin
        // -----------------------

        // Get an IR builder
        // Sets the insertion point to the top of the function
        auto begin_block = &*F.getEntryBlock().getFirstInsertionPt();
        IRBuilder<> BuilderBeg(begin_block);

        // Store llvm value
        llvm::AllocaInst *cycle_beg = new AllocaInst(Int64Ty, 0, "cycle_beg", begin_block);
        llvm::AllocaInst *cycle_end = new AllocaInst(Int64Ty, 0, "cycle_end", begin_block);
        llvm::AllocaInst *core_beg = new AllocaInst(Int64Ty, 0, "core_beg", begin_block);
        llvm::AllocaInst *core_end = new AllocaInst(Int64Ty, 0, "core_end", begin_block);
        llvm::Value *pid = NULL;
        llvm::Value *tid = NULL;

        // Adding at the begin of each modeule's function exept my runtime
        // functions
        if (func_str != rdtscF->getName() && func_str != rdtscpF->getName()
            && func_str != get_pidF->getName() && func_str != get_pidF->getName()
            && func_str != get_num_procsF->getName()
            && func_str != get_num_procs_availableF->getName()) {

          // Call rdtsc to get the clock start
          new StoreInst(zero, cycle_beg, begin_block);
          new StoreInst(zero, cycle_end, begin_block);
          new StoreInst(zero, core_beg, begin_block);
          new StoreInst(zero, core_end, begin_block);
          BuilderBeg.CreateCall(rdtscp, {cycle_beg, core_beg});
        }

        // ---------------------
        // Step 6: Insert at end
        // ---------------------

        for (auto &BB: F) {
          for (auto &II: BB) {
            if (ReturnInst *RI = dyn_cast<ReturnInst>(&II)) {

              // Get an IR builder
              // Sets the insertion point to the top of the function
              IRBuilder<> BuilderEnd(RI);

              if (func_str != rdtscF->getName() && func_str != rdtscpF->getName()
                  && func_str != get_pidF->getName() && func_str != get_pidF->getName()
                  && func_str != get_num_procsF->getName()
                  && func_str != get_num_procs_availableF->getName()) {

                // Call rdtscp to get the clock stop
                BuilderEnd.CreateCall(rdtscp, {cycle_end, core_end});

                // Load clock
                LoadInst *load_cycle_beg =
                  new LoadInst(Int64Ty, cycle_beg, "load_cycle_beg", RI);

                LoadInst *load_cycle_end =
                  new LoadInst(Int64Ty, cycle_end, "load_cycle_end", RI);

                // Call sub to get elpased time
                BinaryOperator *cycle =
                  BinaryOperator::CreateSub(load_cycle_end,
                                            load_cycle_beg,
                                            "elapsed",
                                            RI);

                // Call pid
                pid = BuilderEnd.CreateCall(get_pid);

                // Call tid
                tid = BuilderEnd.CreateCall(get_tid);

                // Update index
                AtomicRMWInst *update_index =
                  new AtomicRMWInst(/* BinOp= */ llvm::AtomicRMWInst::Add,
                                    /* ptr= */ global_count_calls,
                                    /* val= */ one,
                                    /* alignement= */ Align(),
                                    /* ordereing= */ llvm::AtomicOrdering::SequentiallyConsistent,
                                    /* SyncScope= */ SyncScope::System,
                                    /* insert before= */ &*F.getEntryBlock().getFirstInsertionPt());

                // Get string pointer to functin name
                auto FuncName = BuilderBeg.CreateGlobalStringPtr(func_str);

                // Load core ids
                LoadInst *load_core_beg =
                  new LoadInst(Int64Ty, core_beg, "load_core_beg", RI);

                LoadInst *load_core_end =
                  new LoadInst(Int64Ty, core_end, "load_core_end", RI);

                // Store
                BuilderEnd.CreateCall(insert_function,
                                      {update_index, pid, tid, load_core_beg,
                                       load_core_end, cycle, FuncName});
              }

              // ------------------------------------------
              // Step 7: Print the counter of each function
              // ------------------------------------------
              if (func_str == "main") {

                LoadInst *load_index =
                  new LoadInst(Int64Ty, global_count_calls, "load_index", RI);

                // Analyze
                BuilderEnd.CreateCall(analyze_function, load_index);

                // Print info
                BuilderEnd.CreateCall(write_module_summary);
                BuilderEnd.CreateCall(write_function_summary);
                BuilderEnd.CreateCall(write_function_info, load_index);
                BuilderEnd.CreateCall(write_function_info_in_csv_file,
                                      load_index);
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
static RegisterStandardPasses Y(PassManagerBuilder::EP_OptimizerLast,
                                [](const PassManagerBuilder &Builder,
                                   legacy::PassManagerBase &PM) {
                                  PM.add(new InsertRDTSC());
                                });
