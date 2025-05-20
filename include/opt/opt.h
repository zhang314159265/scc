#pragma once

#include "llvm/IR/Module.h"
#include "llvm_opt.h"

namespace scc {

#if USE_LLOPT
void optimize(llvm::Module &M) {
  llvm_opt(M);
}
#else

void optimize(llvm::Module &M) {
  if (getenv("OPT")) {
    M.dump();
    assert(0);
  }
}

#endif

}
