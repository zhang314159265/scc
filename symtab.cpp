#include <assert.h>
#include <iostream>
#include "symtab.h"

Symtab global_symtab;
Symtab* Symtab::cur_ = &global_symtab;

void Symtab::reg(const std::string& id, Type* type) {
  tab_.emplace(id, type);
}

void Symtab::dump() {
  std::cout << "Dump symbol table:" << std::endl;
  for (const auto& [k, v] : tab_) {
    std::cout << k << " " << v->size() << std::endl;
  }
}

Symtab* Symtab::cur() {
  return cur_;
}
