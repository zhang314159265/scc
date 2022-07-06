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

}
