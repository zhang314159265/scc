#pragma once
#include "PostfixExpression.h"
#include "AssignmentOperator.h"
#include "UnaryExpression.h"
#include "EqualityExpression.h"

namespace scc {

#define ConditionalExpression EqualityExpression

class AssignmentExpression {
 public:
  AssignmentExpression() { }
  AssignmentExpression(const ConditionalExpression &cond) : conditional_expression(cond) { }

  AssignmentExpression &addItem(const UnaryExpression &unary, const AssignmentOperator &op) {
    lhsList.emplace_back(unary, op);
    return *this;
  }

 public:
  ConditionalExpression conditional_expression;
  std::vector<std::pair<UnaryExpression, AssignmentOperator>> lhsList;
  friend std::ostream& operator<<(std::ostream& os, const AssignmentExpression& parent) {
    os << "AssignmentExpression" << std::endl;
    os << "RHS: " << parent.conditional_expression;
    os << "NumLHS: " << parent.lhsList.size() << std::endl;
    for (auto &pair : parent.lhsList) {
      os << pair.second << " " << pair.first;
    }
    return os;
  }
};

#undef ConditionalExpression

}
