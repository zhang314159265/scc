#pragma once
#include "semantic_value/PrimaryExpression.h"

namespace scc {

class ArgumentExpressionList;

enum {
  PostfixExpression_NONE,
  PostfixExpression_CALL,
};

class Postfix {
 public:
  Postfix() : tag(PostfixExpression_NONE) { }
  Postfix(const ArgumentExpressionList& argument_expression_list);
 private:
  int tag;
  std::shared_ptr<ArgumentExpressionList> argument_expression_list_ptr;

  friend std::ostream& operator<<(std::ostream& os, const Postfix& p);
};

class PostfixExpression {
 public:
  PostfixExpression() {}
  PostfixExpression(const PrimaryExpression &primary_expression) : primary_expression(primary_expression) { }

  PostfixExpression& addPostfix(const ArgumentExpressionList& argument_expression_list);
 private:
  PrimaryExpression primary_expression;
  std::vector<Postfix> postfixList;
  friend std::ostream& operator<<(std::ostream& os, const PostfixExpression& parent);
};

}
