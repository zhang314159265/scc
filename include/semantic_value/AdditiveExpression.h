#pragma once

namespace scc {

enum AdditiveOp {
  AdditiveOp_ADD,
  AdditiveOp_SUB,
};

static std::ostream &operator<<(std::ostream& os, const AdditiveOp &op) {
  switch (op) {
  case AdditiveOp_ADD:
    os << "+";
    break;
  case AdditiveOp_SUB:
    os << "-";
    break;
  default:
    assert(0);
  }
  return os;
}

#define MultiplicativeExpression UnaryExpression

CreateLeftAssocExprChain(AdditiveExpression, MultiplicativeExpression, AdditiveOp);

#undef MultiplicativeExpression

}
