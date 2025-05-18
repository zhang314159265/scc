#pragma once
#include "ExpressionStatement.h"
#include "JumpStatement.h"

namespace scc {

class CompoundStatement;
class IterationStatement;
class SelectiveStatement;

enum {
  Statement_None,
  Statement_ExpressionStatement,
  Statement_JumpStatement,
  Statement_CompoundStatement,
  Statement_IterationStatement,
  Statement_SelectiveStatement,
};

class Statement {
 public:
  Statement() : tag(Statement_None) { }
  Statement(const ExpressionStatement& expression_statement);
  Statement(const JumpStatement& jump_statement);
  Statement(const CompoundStatement& compound_statement);
  Statement(const IterationStatement& iteration_statement);
  Statement(const SelectiveStatement& selective_statement);
  static const char *tag2str(int tag) {
    switch (tag) {
    case Statement_ExpressionStatement:
      return "Statement_ExpressionStatement";
    case Statement_JumpStatement:
      return "Statement_JumpStatement";
    case Statement_CompoundStatement:
      return "Statement_CompoundStatement";
    case Statement_IterationStatement:
      return "Statement_IterationStatement";
    case Statement_SelectiveStatement:
      return "Statement_SelectiveStatement";
    default:
      assert(0);
    }
  }
 public:
  int tag;
  ExpressionStatement expression_statement;
  JumpStatement jump_statement;
  std::shared_ptr<CompoundStatement> compound_statement;
  // use shared_ptr here to handle circular dependency
  std::shared_ptr<IterationStatement> iteration_statement;
  std::shared_ptr<SelectiveStatement> selective_statement;
  friend std::ostream& operator<<(std::ostream& os, const Statement &parent);
};

}
