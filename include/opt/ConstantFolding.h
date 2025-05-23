#pragma once 

#include "llvm/IR/ConstantFold.h"

// follow ConstantFoldInstruction in include/llvm/Analysis/ConstantFolding.h
namespace scc {

llvm::Constant *ConstantFoldInstOperands(
    llvm::Instruction* I, unsigned OpCode,
    llvm::ArrayRef<llvm::Constant*> Ops) {
  if (llvm::Instruction::isBinaryOp(OpCode)) {
    switch (OpCode) {
    default:
      break;
    case llvm::Instruction::FAdd:
    case llvm::Instruction::FSub:
    case llvm::Instruction::FMul:
    case llvm::Instruction::FDiv:
    case llvm::Instruction::FRem:
      assert(0);
    }
    return llvm::ConstantFoldBinaryInstruction(OpCode, Ops[0], Ops[1]);
  }
  return nullptr;
}

llvm::Constant *ConstantFoldInstruction(llvm::Instruction *I) {
  std::vector<llvm::Constant*> Ops;
  for (llvm::Use &U : I->operands()) {
    auto *cst = llvm::dyn_cast<llvm::Constant>(&U);
    if (!cst) {
      return NULL;
    }
    Ops.push_back(cst);
  }
  return ConstantFoldInstOperands(I, I->getOpcode(), Ops);
}

}
