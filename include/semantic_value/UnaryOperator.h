#pragma once

namespace scc {

enum UnaryOperator {
  UnaryOperator_MINUS,
};

static std::ostream &operator<<(std::ostream &os, const UnaryOperator &op) {
  switch (op) {
  case UnaryOperator_MINUS:
    os << "-";
    break;
  default:
    assert(0);
  }
  return os;
}

}
