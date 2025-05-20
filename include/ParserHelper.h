#pragma once

#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"
#include "semantic_value/TranslationUnit.h"
#include "to_llir/to_llir.h"
#include "opt/opt.h"

namespace scc {

void post_parse(TranslationUnit& tu) {
  llvm::LLVMContext C;
  llvm::Module M("scc generated module", C);

  LowerContext LC = {
    .C = &C,
    .M = &M,
  };
  to_llir(tu, LC);
  optimize(M);

  M.print(llvm::errs(), NULL);
  const std::string &path = "/tmp/gen.ll";
  std::error_code EC;
  llvm::raw_fd_ostream os(path, EC);
  M.print(os, NULL);
  std::cout << "LLIR written to " << path << std::endl;
}

}
