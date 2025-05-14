#pragma once

namespace scc {

enum {
  UnaryExpression_None,
  UnaryExpression_PostfixExpression,
  UnaryExpression_PREINC,
};

// Maybe we could handle unary expression as a postfix-expression/sizeof(type-name) with a list of prefixes.
// But this implementation follow straightly the parse rule.
class UnaryExpression {
 public:
  UnaryExpression() : tag(UnaryExpression_None) { } 
  UnaryExpression(const PostfixExpression &pe) : tag(UnaryExpression_PostfixExpression), postfix_expression(pe) { }
  UnaryExpression(int tag, const UnaryExpression &sub) : tag(tag) {
    unary_expression = std::make_shared<UnaryExpression>(sub);
  }
 public:
  int tag;
  PostfixExpression postfix_expression;
  // shared_ptr rather than unique_ptr to make it easy to copy UnaryExpression
  std::shared_ptr<UnaryExpression> unary_expression;
  friend std::ostream& operator<<(std::ostream& os, const UnaryExpression& parent) {
    os << "UnaryExpression" << std::endl;
    switch (parent.tag) {
    case UnaryExpression_PostfixExpression:
      os << parent.postfix_expression;
      break;
    case UnaryExpression_PREINC:
      os << "++" << *parent.unary_expression;
      break;
    default: 
      assert(0);
    }
    return os;
  }
};

}
