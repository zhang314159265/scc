#include "semantic_value/InitDeclarator.h"

namespace scc {
std::ostream& operator<<(std::ostream& os, const InitDeclarator& init_declarator) {
  os << "InitDecorator" << std::endl;
  os << init_declarator.declarator;
  os << init_declarator.initializer;
  return os;
}

}
