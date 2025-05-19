#pragma once

namespace scc {

enum MultiplicativeOp {
  MultiplicativeOp_MUL,
  MultiplicativeOp_DIV,
  MultiplicativeOp_MOD,
};

static std::ostream &operator<<(std::ostream& os, const MultiplicativeOp &op) {
  switch (op) {
  case MultiplicativeOp_MUL:
    os << "*";
    break;
  case MultiplicativeOp_DIV:
    os << "/";
    break;
  case MultiplicativeOp_MOD:
    os << "%";
    break;
  default:
    assert(0);
  }
  return os;
}

#define CastExpression UnaryExpression

CreateLeftAssocExprChain(MultiplicativeExpression, CastExpression, MultiplicativeOp);

#undef CastExpression

}
