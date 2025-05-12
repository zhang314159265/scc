#include "semantic_value/DeclarationSpecifiers.h"

namespace scc {

std::ostream& operator<<(std::ostream& os, const DeclarationSpecifiers& specs) {
  os << "specs " << specs.speclist.size() << std::endl;
  for (auto spec : specs.speclist) {
    os << type_specifier_to_str(spec) << std::endl;
  }
  os << "quals " << specs.quallist.size() << std::endl;
  for (auto qual : specs.quallist) {
    os << type_qualifier_to_str(qual) << std::endl;
  }
  return os;
}
}

