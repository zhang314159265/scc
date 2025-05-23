#pragma once

#include "llvm/IR/Module.h"
#include "llvm_opt.h"
#include "Mem2RegPass.h"
#include "InstCombinePass.h"

namespace scc {

#if USE_LLOPT
void optimize(llvm::Module &M) {
  llvm_opt(M);
}
#else

void optimize(llvm::Module &M) {
  if (getenv("OPT")) {
    for (auto& F : M) {
      if (!F.isDeclaration()) {
        Mem2RegPass().run(F);
        InstCombinePass().run(F);
      }
    }
  }
}

#endif

}
