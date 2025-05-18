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

llvm::Value *handleAdditive(llvm::Value *lhs, llvm::Value *rhs, AdditiveOp op, LowerContext &LC) {
  llvm::IRBuilder<> &B = *LC.B;
  lhs = derefIfNeeded(lhs, LC);
  rhs = derefIfNeeded(rhs, LC);
  switch (op) {
  case AdditiveOp_ADD:
    return B.CreateAdd(lhs, rhs);
  case AdditiveOp_SUB:
    return B.CreateSub(lhs, rhs);
  default:
    assert(0);
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
  case RelationalOp_GT:
    return B.CreateICmpSGT(lhs, rhs);
  default:
    assert(0);
  }
}

void handleStore(llvm::Value *dst, llvm::Value *val, LowerContext &LC) {
  // do necessary casting before assignment
  llvm::IRBuilder<> &B = *LC.B;

  // double to float casting
  if (val->getType()->isDoubleTy() && llvm::isa<llvm::Constant>(val)) {
    llvm::AllocaInst *alloca = llvm::dyn_cast<llvm::AllocaInst>(dst);
    if (alloca && alloca->getAllocatedType()->isFloatTy()) {
      val = B.CreateFPTrunc(val, alloca->getAllocatedType());
    }
  }
  B.CreateStore(val, dst);
}

void handleAssignment(llvm::Value *lhs, llvm::Value *rhs, AssignmentOperator op, LowerContext &LC) {
  llvm::IRBuilder<> &B = *LC.B;
  if (op == AssignmentOperator::PLUS_EQ) {
    // load lhs value
    llvm::Value *lhsValue = derefIfNeeded(lhs, LC);
    llvm::Value *rhsValue = derefIfNeeded(rhs, LC);
    llvm::Value *result = B.CreateAdd(lhsValue, rhsValue);
    handleStore(lhs, result, LC);
    return;
  }
  if (op == AssignmentOperator::EQ) {
    llvm::Value *rhsValue = derefIfNeeded(rhs, LC);
    handleStore(lhs, rhsValue, LC);
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

void handleCastIfPrintf(llvm::Function *function, std::vector<llvm::Value*> &args, LowerContext &LC) {
  // cast float arg to double for printf
  llvm::LLVMContext &C = *LC.C;
  llvm::IRBuilder<> &B = *LC.B;
  if (function->getName() == "printf") {
    for (int i = 0; i < args.size(); ++i) {
      llvm::Value *arg = derefIfNeeded(args[i], LC);
      if (arg->getType()->isFloatTy()) {
        args[i] = B.CreateFPExt(arg, llvm::Type::getDoubleTy(C));
      }
    }
  }
}

}
