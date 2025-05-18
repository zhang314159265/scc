#pragma once

namespace scc {

enum {
  SelectiveStatement_None,
  SelectiveStatement_IFELSE,
};

class SelectiveStatement {
 public:
  SelectiveStatement() : tag(SelectiveStatement_None) { }
  SelectiveStatement(int tag, const Expression &expr, const Statement &stmt1, const Statement &stmt2) : tag(tag), expr(expr), stmt1(stmt1), stmt2(stmt2) { }
 public:
  int tag;
  Expression expr;
  Statement stmt1, stmt2;
  friend std::ostream& operator<<(std::ostream& os, const SelectiveStatement& parent) {
    os << "SelectiveStatement" << std::endl;
    switch (parent.tag) {
    case SelectiveStatement_IFELSE:
      os << "IfElse:" << std::endl;
      os << "Expr: " << parent.expr;
      os << "Stmt1: " << parent.stmt1;
      os << "Stmt2: " << parent.stmt2;
      break;
    default:
      assert(0);
    }
    return os;

  }
};

}
