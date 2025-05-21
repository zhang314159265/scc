#pragma once

#include "llvm/IR/Module.h"
#include "llvm_opt.h"
#include "Mem2RegPass.h"

namespace scc {

#if USE_LLOPT
void optimize(llvm::Module &M) {
  llvm_opt(M);
}
#else

void optimize(llvm::Module &M) {
  if (getenv("OPT")) {
    Mem2RegPass().run(M);
  }
}

#endif

}
