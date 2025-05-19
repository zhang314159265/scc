#pragma once
#include "PrimaryExpression.h"

namespace scc {

class ArgumentExpressionList;
class Expression;

enum {
  PostfixExpression_NONE,
  PostfixExpression_CALL,
  PostfixExpression_INDEX, // for array access
};

class Postfix {
 public:
  Postfix() : tag(PostfixExpression_NONE) { }
  Postfix(const ArgumentExpressionList& argument_expression_list);
  Postfix(const Expression &expression);
 public:
  int tag;
  std::shared_ptr<ArgumentExpressionList> argument_expression_list_ptr;
  std::shared_ptr<Expression> expression;

  friend std::ostream& operator<<(std::ostream& os, const Postfix& p);
};

class PostfixExpression {
 public:
  PostfixExpression() {}
  PostfixExpression(const PrimaryExpression &primary_expression) : primary_expression(primary_expression) { }

  PostfixExpression& addPostfix(const ArgumentExpressionList& argument_expression_list);
  PostfixExpression& addPostfix(const Expression& expr);
 public:
  PrimaryExpression primary_expression;
  std::vector<Postfix> postfixList;
  friend std::ostream& operator<<(std::ostream& os, const PostfixExpression& parent);
};

}
