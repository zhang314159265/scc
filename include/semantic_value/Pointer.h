#pragma once
#include "semantic_value/TypeQualifierList.h"

namespace scc {

class Pointer {
 public:
  Pointer() { }
  Pointer(const TypeQualifierList &quallist) {
    quallists.push_back(quallist);
  }

  Pointer &addTypeQualifierList(const TypeQualifierList &quallist) {
    quallists.push_back(quallist);
    return *this;
  }
 public:
  std::vector<TypeQualifierList> quallists;
  friend std::ostream& operator<<(std::ostream& os, const Pointer& pointer);
};

}
