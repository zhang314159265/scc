#pragma once

#include <iostream>
#include "InitDeclaratorList.h"
#include "semantic_value/DeclarationSpecifiers.h"

namespace scc {
CreateComposeType2Decl(Declaration,
    DeclarationSpecifiers, declaration_specifiers,
    InitDeclaratorList, init_declarator_list)
}
