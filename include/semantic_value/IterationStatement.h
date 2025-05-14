#pragma once

#include "Statement.h"

namespace scc {

enum {
  IterationStatement_None,
  IterationStatement_FOR,
};

class IterationStatement {
 public:
  IterationStatement() : tag(IterationStatement_None) { }
  IterationStatement(int tag, const Expression& expr1, const Expression &expr2, const Expression& expr3, const Statement &stmt) : tag(tag), expr1(expr1), expr2(expr2), expr3(expr3), stmt(stmt) { }
 public:
  int tag;
  Expression expr1, expr2, expr3;
  Statement stmt;
  friend std::ostream& operator<<(std::ostream& os, const IterationStatement& parent) {
    os << "IterationStatement" << std::endl;
    switch (parent.tag) {
    case IterationStatement_FOR:
      os << "For:" << std::endl;
      os << "Expr1: " << parent.expr1;
      os << "Expr2: " << parent.expr2;
      os << "Expr3: " << parent.expr3;
      os << "Stmt: " << parent.stmt;
      break;
    default:
      assert(0);
    }
    return os;
  }
};

}
