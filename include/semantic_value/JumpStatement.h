#pragma once

namespace scc {

enum {
  JumpStatement_None,
  JumpStatement_RETURN,
};

class JumpStatement {
 public:
  JumpStatement() : tag(JumpStatement_None) {}
  JumpStatement(const Expression &expression) : tag(JumpStatement_RETURN), expression(expression) { }
 private:
  int tag;
  Expression expression;
  friend std::ostream& operator<<(std::ostream& os, const JumpStatement& parent) {
    os << "JumpStatement" << std::endl;
    switch (parent.tag) {
    case JumpStatement_RETURN:
      os << "return: ";
      os << parent.expression;
      break;
    default:
      assert(0);
    }
    return os;
  }
};

}
