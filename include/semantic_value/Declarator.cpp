#include "semantic_value/Declarator.h"

namespace scc {
std::ostream& operator<<(std::ostream& os, const Declarator& declarator) {
  os << declarator.pointer;
  os << declarator.direct_declarator;
  return os;
}


}
