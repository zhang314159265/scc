#pragma once

#include <vector>

namespace scc {

class DeclarationList;

class DirectDeclarator {
 public:
  explicit DirectDeclarator(std::string _identifier = "") : identifier(_identifier) { }

  DirectDeclarator& addDeclarationList(const DeclarationList& decl_list) {
    decl_list_list.push_back(decl_list);
    return *this;
  }
 public:
  std::string identifier;
  std::vector<DeclarationList> decl_list_list;

  friend std::ostream& operator<<(std::ostream&, const DirectDeclarator&);
};

}
