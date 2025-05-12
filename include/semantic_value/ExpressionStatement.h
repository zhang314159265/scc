#pragma once
#include "semantic_value/Expression.h"

namespace scc {

class ExpressionStatement {
 public:
  ExpressionStatement() { }
  ExpressionStatement(const Expression& expression) : expression(expression) { }
 private:
  Expression expression;
  friend std::ostream& operator<<(std::ostream& os, const ExpressionStatement& parent) {
    os << "ExpressionStatement" << std::endl;
    os << parent.expression;
    return os;
  }
};

}
