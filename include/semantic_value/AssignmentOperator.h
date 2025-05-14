#pragma once

namespace scc {

enum AssignmentOperator {
  PLUS_EQ,
  EQ,
};

static std::ostream &operator<<(std::ostream& os, const AssignmentOperator &op) {
  switch (op) {
  case PLUS_EQ:
    os << "+=";
    break;
  case EQ:
    os << "=";
    break;
  default:
    assert(0);
  }
  return os;
}

}
