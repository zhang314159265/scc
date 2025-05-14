#pragma once
#include "Declarator.h"
#include "Pointer.h"
#include "Initializer.h"

namespace scc {

class InitDeclarator {
 public:
  InitDeclarator() {}

  // Initializer is None initialized
  InitDeclarator(const Declarator &_declarator) : declarator(_declarator), initializer() { }

  InitDeclarator(const Declarator &_declarator, const Initializer &init) : declarator(_declarator), initializer(init) { }

 public:
  Declarator declarator;
  Initializer initializer;

  friend std::ostream& operator<<(std::ostream& os, const InitDeclarator& init_declarator);
};



}
