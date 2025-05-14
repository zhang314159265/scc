#pragma once

namespace scc {

#define ShiftExpression UnaryExpression

enum RelationalOp {
  RelationalOp_LE,
};

static std::ostream &operator<<(std::ostream& os, const RelationalOp &op) {
  switch (op) {
  case RelationalOp_LE:
    os << "<=";
    break;
  default:
    assert(0);
  }
  return os;
}

CreateLeftAssocExprChain(RelationalExpression, ShiftExpression, RelationalOp);

#undef ShiftExpression

}
