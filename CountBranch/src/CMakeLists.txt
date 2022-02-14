add_llvm_library( LLVMCountBranch MODULE BUILDTREE_ONLY
  CountBranch.cpp

  DEPENDS
  intrinsics_gen
  PLUGIN_TOOL
  opt
  )
