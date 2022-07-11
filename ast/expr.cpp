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

}
