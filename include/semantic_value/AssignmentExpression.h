#pragma once
#include "PostfixExpression.h"

namespace scc {

#define ConditionalExpression PostfixExpression

CreateWrapperType(AssignmentExpression, ConditionalExpression, conditional_expression);

#undef ConditionalExpression

}
