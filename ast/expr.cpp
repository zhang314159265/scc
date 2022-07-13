#include "ast/expr.h"
#include "emitter.h"

namespace ast {

std::unique_ptr<ExprBase> CommaExpr::gen(Emitter* emitter) {
  std::unique_ptr<ExprBase> last = nullptr;
  for (auto& itemptr : list_) {
    last = itemptr->gen(emitter);
  }
  return last;
}

void CommaExpr::jumping(Emitter* emitter, Label* trueLabel, Label* falseLabel) {
  assert(list_.size() > 0); // must have at least one expression
  // the value of the final expression is used
  for (int i = 0; i < list_.size() - 1; ++i) {
    list_[i]->gen(emitter);
  }
  list_.back()->jumping(emitter, trueLabel, falseLabel);
}

std::unique_ptr<ExprBase> BinOpExpr::gen(Emitter* emitter) {
  auto lhs_addr = lhs_->gen(emitter);
  auto rhs_addr = rhs_->gen(emitter);
  auto temp = emitter->createTemp();
  emitter->emit(
    fmt::format("{} = {} {} {}",
      temp->getAddrStr(),
      lhs_addr->getAddrStr(),
      opstr_,
      rhs_addr->getAddrStr()));
  return temp;
}

void Id::jumping(Emitter* emitter, Label* trueLabel, Label* falseLabel) {
  auto testStr = fmt::format("{} != 0", str_);
  if (trueLabel && falseLabel) {
    emitter->emitCondJump(testStr, trueLabel, false);
    emitter->emitJump(falseLabel);
  } else if (trueLabel) {
    emitter->emitCondJump(testStr, trueLabel, false);
  } else if (falseLabel) {
    emitter->emitCondJump(testStr, falseLabel, true);
  } else {
    // do nothing. just fallthru
  }
}

std::unique_ptr<ExprBase> AssignExpr::gen(Emitter* emitter) {
  // TODO we should check lhs represents lvalue
  auto rhs_addr = rhs_->gen(emitter);
  emitter->emit(
    fmt::format("{} = {}",
      lhs_->getAddrStr(),
      rhs_addr->getAddrStr()));
  return rhs_addr;
}

std::unique_ptr<ExprBase> IncDec::gen(Emitter* emitter) {
  std::unique_ptr<ExprBase> ret = nullptr;
  if (is_pre_) {
    // return a clone of child_ . Theoretically we can
    // 1. either make the return value a shared_ptr
    // 2. or add API to clone an expression.
    // We pick latter for now.
    ret = child_->clone();
  } else {
    // return a Temp storing the original value
    ret = emitter->createTemp();

    // emit the assignment
    emitter->emit(
      fmt::format("{} = {}",
        ret->getAddrStr(),
        child_->getAddrStr())); // TODO support cases like array element/pointer deference
  }
  // TODO need handle pointer inc/dec specially considering type size
  emitter->emit(
    fmt::format("{} = {} {} 1", child_->getAddrStr(), child_->getAddrStr(), opstr()));
  return ret;
}

}
