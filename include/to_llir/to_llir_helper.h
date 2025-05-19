#pragma once

#include <algorithm>

namespace scc {

#define BINARY_CREATOR_FACTORY(Name) \
  llvm::Value *Create ## Name(llvm::Value *lhs, llvm::Value *rhs, llvm::IRBuilder<> &B) { \
    if (lhs->getType()->isFloatingPointTy()) { \
      return B.CreateF ## Name(lhs, rhs); \
    } else { \
      return B.Create ## Name(lhs, rhs); \
    } \
  }

BINARY_CREATOR_FACTORY(Add)
BINARY_CREATOR_FACTORY(Sub)
BINARY_CREATOR_FACTORY(Mul)

llvm::Value *CreateDiv(llvm::Value *lhs, llvm::Value *rhs, llvm::IRBuilder<> &B) {
  if (lhs->getType()->isFloatingPointTy()) {
    return B.CreateFDiv(lhs, rhs);
  } else {
    return B.CreateUDiv(lhs, rhs);
  }
}

llvm::Value *CreateMod(llvm::Value *lhs, llvm::Value *rhs, llvm::IRBuilder<> &B) {
  if (lhs->getType()->isFloatingPointTy()) {
    return B.CreateFRem(lhs, rhs);
  } else {
    return B.CreateURem(lhs, rhs);
  }
}

llvm::Value *handleImplicitCast(llvm::Value *val, llvm::Type *dstTy, LowerContext &LC);

llvm::Type *getRealType(llvm::Value *val) {
  if (llvm::isa<llvm::AllocaInst>(val)) {
    llvm::AllocaInst *inst = llvm::cast<llvm::AllocaInst>(val);
    return inst->getAllocatedType();
  }
  if (llvm::isa<llvm::GetElementPtrInst>(val)) {
    llvm::GetElementPtrInst *inst = llvm::cast<llvm::GetElementPtrInst>(val);
    return inst->getResultElementType();
  }

  return val->getType(); 
}

llvm::Value *derefIfNeeded(llvm::Value *valueInp, LowerContext &LC) {
  if (llvm::isa<llvm::AllocaInst>(valueInp)) {
    llvm::IRBuilder<> &B = *LC.B;
    llvm::AllocaInst *inst = llvm::cast<llvm::AllocaInst>(valueInp);

    return B.CreateLoad(inst->getAllocatedType(), valueInp);
  } else if (llvm::isa<llvm::GetElementPtrInst>(valueInp)) {
    llvm::IRBuilder<> &B = *LC.B;
    llvm::GetElementPtrInst *inst = llvm::cast<llvm::GetElementPtrInst>(valueInp);

    return B.CreateLoad(inst->getResultElementType(), valueInp);
  } else {
    return valueInp;
  }
}

llvm::Value *handleMultiplicative(llvm::Value *lhs, llvm::Value *rhs, MultiplicativeOp op, LowerContext &LC) {
  llvm::IRBuilder<> &B = *LC.B;
  lhs = derefIfNeeded(lhs, LC);
  rhs = derefIfNeeded(rhs, LC);
  switch (op) {
  case MultiplicativeOp_MUL:
    return CreateMul(lhs, rhs, B);
  case MultiplicativeOp_DIV:
    return CreateDiv(lhs, rhs, B);
  case MultiplicativeOp_MOD:
    return CreateMod(lhs, rhs, B);
  default:
    assert(0);
  }
}

llvm::Value *handleAdditive(llvm::Value *lhs, llvm::Value *rhs, AdditiveOp op, LowerContext &LC) {
  llvm::IRBuilder<> &B = *LC.B;
  lhs = derefIfNeeded(lhs, LC);
  rhs = derefIfNeeded(rhs, LC);
  switch (op) {
  case AdditiveOp_ADD:
    return CreateAdd(lhs, rhs, B);
  case AdditiveOp_SUB:
    return CreateSub(lhs, rhs, B);
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
  case RelationalOp_LT:
    return B.CreateICmpSLT(lhs, rhs);
  case RelationalOp_GT:
    return B.CreateICmpSGT(lhs, rhs);
  default:
    assert(0);
  }
}

void handleStore(llvm::Value *dst, llvm::Value *val, LowerContext &LC) {
  // do necessary casting before assignment
  llvm::IRBuilder<> &B = *LC.B;

  val = handleImplicitCast(val, getRealType(dst), LC);
  B.CreateStore(val, dst);
}

void handleAssignment(llvm::Value *lhs, llvm::Value *rhs, AssignmentOperator op, LowerContext &LC) {
  llvm::IRBuilder<> &B = *LC.B;
  if (op == AssignmentOperator::PLUS_EQ) {
    // load lhs value
    llvm::Value *lhsValue = derefIfNeeded(lhs, LC);
    llvm::Value *rhsValue = handleImplicitCast(derefIfNeeded(rhs, LC), lhsValue->getType(), LC);
    llvm::Value *result = CreateAdd(lhsValue, rhsValue, B);
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

// return true if this is a call for printf
bool handleCastIfPrintf(llvm::Function *function, std::vector<llvm::Value*> &args, LowerContext &LC) {
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
    return true;
  }
  return false;
}

llvm::Value *handleImplicitCast(llvm::Value *val, llvm::Type *dstTy, LowerContext &LC) {
  if (val->getType() == dstTy) {
    return val;
  }

  // i32 -> double
  if (val->getType()->isIntegerTy(32) && dstTy->isFloatingPointTy()) {
    llvm::IRBuilder<> &B = *LC.B;
    return B.CreateSIToFP(val, dstTy);
  }

  // double -> float
  if (val->getType()->isDoubleTy() && dstTy->isFloatTy()) {
    llvm::IRBuilder<> &B = *LC.B;
    return B.CreateFPTrunc(val, dstTy);
  }

  assert(0);
}

void handleCastForCall(llvm::Function *function, std::vector<llvm::Value*> &args, LowerContext &LC) {
  auto funcTy = function->getFunctionType();
  if (funcTy->isVarArg()) {
    return;
  }
  assert(funcTy->getNumParams() == args.size());
  for (int i = 0; i < args.size(); ++i) {
    args[i] = handleImplicitCast(args[i], funcTy->getParamType(i), LC);
  }
}

}
