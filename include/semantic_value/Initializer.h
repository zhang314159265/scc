#pragma once

#include "AssignmentExpression.h"

namespace scc {

enum {
  Initializer_None,
  Initializer_Expr,
  Initializer_List,
};

class Initializer {
 public:
  Initializer() : tag(Initializer_None) { }
  Initializer(const AssignmentExpression &expr) : tag(Initializer_Expr), assignment_expression(expr) { }
 public:
  int tag;
  AssignmentExpression assignment_expression;
  friend std::ostream& operator<<(std::ostream& os, const Initializer& parent) {
    std::cout << "Initializer" << std::endl;
    switch (parent.tag) {
    case Initializer_Expr:
      std::cout << parent.assignment_expression;
      break;
    case Initializer_None:
      std::cout << "none initializer" << std::endl;
      break;
    default:
      assert(0);
    }
    return os;
  }
};

}
