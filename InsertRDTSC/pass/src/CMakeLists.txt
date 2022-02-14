add_llvm_library( LLVMInsertRDTSC MODULE BUILDTREE_ONLY
  InsertRDTSC.cpp

  DEPENDS
  intrinsics_gen
  PLUGIN_TOOL
  opt
  )
