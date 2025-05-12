#pragma once
#include "semantic_value/Declarator.h"
#include "semantic_value/Pointer.h"

namespace scc {

class InitDeclarator {
 public:
  InitDeclarator() {}
  InitDeclarator(Declarator _declarator) : declarator(_declarator) { }

 private:
  Declarator declarator;

  friend std::ostream& operator<<(std::ostream& os, const InitDeclarator& init_declarator);
};



}
