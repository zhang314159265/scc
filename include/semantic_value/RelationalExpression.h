#pragma once

#include "AdditiveExpression.h"

namespace scc {

#define ShiftExpression AdditiveExpression

enum RelationalOp {
  RelationalOp_LE,
  RelationalOp_LT,
  RelationalOp_GT,
};

static std::ostream &operator<<(std::ostream& os, const RelationalOp &op) {
  switch (op) {
  case RelationalOp_LT:
    os << "<";
    break;
  case RelationalOp_LE:
    os << "<=";
    break;
  case RelationalOp_GT:
    os << ">";
    break;
  default:
    assert(0);
  }
  return os;
}

CreateLeftAssocExprChain(RelationalExpression, ShiftExpression, RelationalOp);

#undef ShiftExpression

}
