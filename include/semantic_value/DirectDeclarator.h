#pragma once

#include <vector>

namespace scc {

class DeclarationList;

#define ConditionalExpression EqualityExpression
class ConditionalExpression;

class DirectDeclarator {
 public:
  explicit DirectDeclarator(std::string _identifier = "") : identifier(_identifier) { }

  DirectDeclarator& addDeclarationList(const DeclarationList& decl_list) {
    decl_list_list.push_back(decl_list);
    return *this;
  }

  DirectDeclarator& addConstantExpression(const ConditionalExpression& expr) {
    constant_expressions.push_back(std::make_shared<ConditionalExpression>(expr));
    return *this;
  }

 public:
  std::string identifier;
  std::vector<DeclarationList> decl_list_list;
  // handle circular dependencies
  std::vector<std::shared_ptr<ConditionalExpression>> constant_expressions;

  friend std::ostream& operator<<(std::ostream&, const DirectDeclarator&);
};

#undef ConditionalExpression

}
