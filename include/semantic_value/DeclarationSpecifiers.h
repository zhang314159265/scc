#pragma once

#include <vector>

#include "TypeSpecifier.h"
#include "TypeQualifier.h"

namespace scc {

// The order of TypeSpecfier/TypeQualifier does not matter,
// so using two vectors (ont for TypeSpecifier, one for TypeQualifier)
// is enough.
class DeclarationSpecifiers {
 public:
  DeclarationSpecifiers& addSpecifier(TypeSpecifier spec) {
    speclist.push_back(spec);
    return *this;
  }

  DeclarationSpecifiers& addQualifier(TypeQualifier qual) {
    quallist.push_back(qual);
    return *this;
  }
 public:
  std::vector<TypeSpecifier> speclist;
  std::vector<TypeQualifier> quallist;
  friend std::ostream& operator<<(std::ostream&, const DeclarationSpecifiers& specs);
};


}
