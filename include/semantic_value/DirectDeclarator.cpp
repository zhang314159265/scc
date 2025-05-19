#include <iostream>

#include "DirectDeclarator.h"
#include "RelationalExpression.h"

namespace scc {

std::ostream& operator<<(std::ostream& os, const DirectDeclarator& d) {
  os << "DirectDeclarator" << std::endl;
  os << d.identifier << std::endl;
  os << "num decl list " << d.decl_list_list.size() << std::endl;
  for (auto &decl_list : d.decl_list_list) {
    os << decl_list << std::endl;
  }
  os << "num const expr " << d.constant_expressions.size() << std::endl;
  for (auto &expr : d.constant_expressions) {
    os << *expr << std::endl;
  }
  return os;
}

}
