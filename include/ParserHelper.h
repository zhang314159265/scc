#pragma once

#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Type.h"

namespace scc {

llvm::LLVMContext &_C() {
  static llvm::LLVMContext ctx;
  return ctx;
}

}
