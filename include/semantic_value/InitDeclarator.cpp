#include "semantic_value/InitDeclarator.h"

namespace scc {
std::ostream& operator<<(std::ostream& os, const InitDeclarator& init_declarator) {
  os << init_declarator.declarator;
  return os;
}

}
