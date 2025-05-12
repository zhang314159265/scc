#include "PostfixExpression.h"
#include "ArgumentExpressionList.h"

namespace scc {

Postfix::Postfix(const ArgumentExpressionList& argument_expression_list) : tag(PostfixExpression_CALL) {
  argument_expression_list_ptr = std::make_shared<ArgumentExpressionList>(argument_expression_list);
}

std::ostream& operator<<(std::ostream& os, const Postfix& p) {
  std::cout << "Postfix" << std::endl;
  switch (p.tag) {
  case PostfixExpression_CALL:
    os << *(p.argument_expression_list_ptr) << std::endl;
    break;
  default:
    assert(0);
  }
  return os;
}

std::ostream& operator<<(std::ostream& os, const PostfixExpression& parent) {
  os << "PostfixExpression" << std::endl;
  os << parent.primary_expression << std::endl;

  if (parent.postfixList.size() > 0) {
    os << "num postfix " << parent.postfixList.size() << std::endl;
    for (const auto &postfix : parent.postfixList) {
      os << postfix << std::endl;
    }
  }
  return os;
}

PostfixExpression& PostfixExpression::addPostfix(const ArgumentExpressionList& argument_expression_list) {
  postfixList.emplace_back(argument_expression_list);
  return *this;
}

}
