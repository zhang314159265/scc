#pragma once

#include "RelationalExpression.h"

namespace scc {

enum EqualityOp {
  EqualityOp_EQ,
  EqualityOp_NEQ,
};

static std::ostream &operator<<(std::ostream& os, const EqualityOp &op) {
  switch (op) {
  case EqualityOp_EQ:
    os << "==";
    break;
  case EqualityOp_NEQ:
    os << "!=";
    break;
  default:
    assert(0);
  }
  return os;
}


CreateLeftAssocExprChain(EqualityExpression, RelationalExpression, EqualityOp);

}
