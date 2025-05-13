#pragma once
#include "Pointer.h"
#include "DirectDeclarator.h"

namespace scc {

class Declarator {
 public:
  Declarator() {}
  Declarator(const Pointer &pointer, const DirectDeclarator &direct_declarator) {
    this->pointer = pointer;
    this->direct_declarator = direct_declarator;
  }
 public:
  Pointer pointer;
  DirectDeclarator direct_declarator;
  friend std::ostream& operator<<(std::ostream& os, const Declarator& declarator);
};
 
}

