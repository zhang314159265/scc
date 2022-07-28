#pragma once

#include <string>
#include <unordered_map>
#include "type.h"

class Symtab {
 public:
  void reg(const std::string& id, Type* type);
  Type* getType(const std::string& id) {
    auto itr = tab_.find(id);
    assert(itr != tab_.end());
    return itr->second;
  }
  void dump();
  static Symtab* cur();
 private:
  // TODO: create an Entry type for table value?
  std::unordered_map<std::string, Type*> tab_;
  static Symtab* cur_;
};
