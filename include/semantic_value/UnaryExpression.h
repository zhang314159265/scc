#pragma once

#include "UnaryOperator.h"

namespace scc {

enum {
  UnaryExpression_None,
  UnaryExpression_PostfixExpression,
  UnaryExpression_PREINC,
  UnaryExpression_UNARY_OP,
};

#define CastExpression UnaryExpression

// Maybe we could handle unary expression as a postfix-expression/sizeof(type-name) with a list of prefixes.
// But this implementation follow straightly the parse rule.
class UnaryExpression {
 public:
  UnaryExpression() : tag(UnaryExpression_None) { } 
  UnaryExpression(const PostfixExpression &pe) : tag(UnaryExpression_PostfixExpression), postfix_expression(pe) { }
  UnaryExpression(int tag, const UnaryExpression &sub) : tag(tag) {
    unary_expression = std::make_shared<UnaryExpression>(sub);
  }

  UnaryExpression(UnaryOperator unaryOp, const CastExpression &castExpr) : tag(UnaryExpression_UNARY_OP), unaryOp(unaryOp) {
    this->cast_expression = std::make_shared<CastExpression>(castExpr);
  }
 public:
  int tag;

  // fields for UnaryExpression_PostfixExpression
  PostfixExpression postfix_expression;

  // fields for UnaryExpression_PREINC
  // shared_ptr rather than unique_ptr to make it easy to copy UnaryExpression
  std::shared_ptr<UnaryExpression> unary_expression;

  // fields for UnaryExpression_UNARY_OP
  UnaryOperator unaryOp;
  std::shared_ptr<CastExpression> cast_expression;

  friend std::ostream& operator<<(std::ostream& os, const UnaryExpression& parent) {
    os << "UnaryExpression" << std::endl;
    switch (parent.tag) {
    case UnaryExpression_PostfixExpression:
      os << parent.postfix_expression;
      break;
    case UnaryExpression_PREINC:
      os << "++" << *parent.unary_expression;
      break;
    case UnaryExpression_UNARY_OP:
      os << parent.unaryOp << *parent.cast_expression;
      break;
    default: 
      std::cout << "unsupported tag " << parent.tag << std::endl;
      assert(0);
    }
    return os;
  }
};

#undef CastExpression

}
