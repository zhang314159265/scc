#include "Statement.h"
#include "CompoundStatement.h"
#include "IterationStatement.h"

namespace scc {

Statement::Statement(const ExpressionStatement& expression_statement) : tag(Statement_ExpressionStatement), expression_statement(expression_statement) { }

Statement::Statement(const JumpStatement& jump_statement) : tag(Statement_JumpStatement), jump_statement(jump_statement) { }

Statement::Statement(const CompoundStatement& compound_statement) : tag(Statement_CompoundStatement) { 
  this->compound_statement = std::make_shared<CompoundStatement>(compound_statement);
}

Statement::Statement(const IterationStatement& iteration_statement) : tag(Statement_IterationStatement) { 
  this->iteration_statement = std::make_shared<IterationStatement>(iteration_statement);
}

std::ostream& operator<<(std::ostream& os, const Statement& parent) {
  os << "Statement(tag " << Statement::tag2str(parent.tag) << ")" << std::endl;
  switch (parent.tag) {
  case Statement_ExpressionStatement:
    os << parent.expression_statement;
    break;
  case Statement_JumpStatement:
    os << parent.jump_statement;
    break;
  case Statement_CompoundStatement:
    os << *parent.compound_statement;
    break;
  case Statement_IterationStatement:
    os << *parent.iteration_statement;
    break;
  default:
    assert(0);
  }
  return os;
}

}
