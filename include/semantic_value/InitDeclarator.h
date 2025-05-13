#pragma once
#include "Declarator.h"
#include "semantic_value/Pointer.h"

namespace scc {

class InitDeclarator {
 public:
  InitDeclarator() {}
  InitDeclarator(Declarator _declarator) : declarator(_declarator) { }

 public:
  Declarator declarator;

  friend std::ostream& operator<<(std::ostream& os, const InitDeclarator& init_declarator);
};



}
