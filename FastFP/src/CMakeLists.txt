add_llvm_library( LLVMFastFP MODULE BUILDTREE_ONLY
  FastFP.cpp

  DEPENDS
  intrinsics_gen
  PLUGIN_TOOL
  opt
  )
