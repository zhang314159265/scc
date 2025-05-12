#pragma once
#include "semantic_value/ExpressionStatement.h"
#include "semantic_value/JumpStatement.h"

namespace scc {
CreateUnionType2(Statement,
  ExpressionStatement, expression_statement,
  JumpStatement, jump_statement);
}
