#include "semantic_value/Pointer.h"

namespace scc {

std::ostream& operator<<(std::ostream& os, const Pointer& pointer) {
  os << "Pointer " << pointer.quallists.size() << std::endl;
  for (auto &quallist : pointer.quallists) {
    os << quallist << std::endl;
  }
  return os;
}

}
