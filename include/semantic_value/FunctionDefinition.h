#pragma once
#include "semantic_value/CompoundStatement.h"

namespace scc {

CreateComposeType3(FunctionDefinition,
  DeclarationSpecifiers, declaration_specifiers,
  Declarator, declarator,
  CompoundStatement, compound_statement);

}

