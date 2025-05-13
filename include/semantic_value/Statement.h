#pragma once
#include "ExpressionStatement.h"
#include "JumpStatement.h"

namespace scc {
CreateUnionType2(Statement,
  ExpressionStatement, expression_statement,
  JumpStatement, jump_statement);
}
