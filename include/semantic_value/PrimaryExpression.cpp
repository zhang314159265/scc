#include "PrimaryExpression.h"

namespace scc {

std::ostream& operator<<(std::ostream& os, const PrimaryExpression& parent) {
  os << "PrimaryExpression: (tag " << PrimaryExpression::tag2str(parent.tag) << ")" << std::endl;
  switch (parent.tag) {
  case PrimaryExpression_IDENTIFIER:
  case PrimaryExpression_STR:
    os << parent.str << std::endl;
    break;
  case PrimaryExpression_constant:
    os << parent.constant << std::endl;
    break;
  default:
    assert(0);
  }
  return os;
}

}
