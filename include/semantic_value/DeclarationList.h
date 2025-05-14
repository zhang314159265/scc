#pragma once
#include "semantic_value/Declaration.h"

namespace scc { 

// copied from TypeCreator.h to add an extra field
#define CreateDeclarationListType2Decl(ListType, ItemTypeName) \
  class ListType { \
   public: \
    ListType() {} \
    ListType(const ItemTypeName& item) { \
      items.push_back(item); \
    } \
    \
    ListType& addItem(const ItemTypeName& item) { \
      items.push_back(item); \
      return * this; \
    } \
   public: \
    std::vector<ItemTypeName> items; \
    bool hasEllipsis = false; \
    friend std::ostream& operator<<(std::ostream& os, const ListType& list); \
  };

CreateDeclarationListType2Decl(DeclarationList, Declaration)

}
