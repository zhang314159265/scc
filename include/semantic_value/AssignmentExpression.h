#pragma once
#include "semantic_value/PostfixExpression.h"

namespace scc {

#define ConditionalExpression PostfixExpression

CreateWrapperType(AssignmentExpression, ConditionalExpression, conditional_expression);

}
