#pragma once

#include <algorithm>

namespace scc {

llvm::Value *derefIfNeeded(llvm::Value *valueInp, LowerContext &LC) {
  if (llvm::isa<llvm::AllocaInst>(valueInp)) {
    llvm::IRBuilder<> &B = *LC.B;
    llvm::AllocaInst *inst = llvm::cast<llvm::AllocaInst>(valueInp);

    return B.CreateLoad(inst->getAllocatedType(), valueInp);
  } else {
    return valueInp;
  }
}

llvm::Value *handleRelational(llvm::Value *lhs, llvm::Value *rhs, RelationalOp op, LowerContext &LC) {
  llvm::IRBuilder<> &B = *LC.B;
  lhs = derefIfNeeded(lhs, LC);
  rhs = derefIfNeeded(rhs, LC);
  switch (op) {
  case RelationalOp_LE:
    // TODO this only works for signed integer?
    return B.CreateICmpSLE(lhs, rhs);
  default:
    assert(0);
  }
}

void handleAssignment(llvm::Value *lhs, llvm::Value *rhs, AssignmentOperator op, LowerContext &LC) {
  llvm::IRBuilder<> &B = *LC.B;
  if (op == AssignmentOperator::PLUS_EQ) {
    // load lhs value
    llvm::Value *lhsValue = derefIfNeeded(lhs, LC);
    llvm::Value *rhsValue = derefIfNeeded(rhs, LC);
    llvm::Value *result = B.CreateAdd(lhsValue, rhsValue);
    B.CreateStore(result, lhs);
    return;
  }
  if (op == AssignmentOperator::EQ) {
    llvm::Value *rhsValue = derefIfNeeded(rhs, LC);
    B.CreateStore(rhsValue, lhs);
    return;
  }
  assert(0); 
}

llvm::Value *lookupIdentifier(std::string &name, LowerContext &LC) {
  llvm::Function *F = LC.F;
  llvm::Module &M = *LC.M;
  llvm::Value *found = nullptr;

  if (F) {
    // lookup local variable
    found = F->getValueSymbolTable()->lookup(name);
  }
  if (!found) {
    // lookup global variable
    found = M.getNamedValue(name);
  }
  assert(found);
  return found;
}

std::string replaceAll(std::string str, const std::string &from, const std::string &to) {
  int start_pos = 0;
  while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
    str.replace(start_pos, from.length(), to);
    start_pos += to.length();
  }
  return str;
}

std::string handleStringEscape(const std::string &s) {
  std::string out = replaceAll(s, "\\n", "\n");
  return out;
}

}
