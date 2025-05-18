#include "DeclarationList.h"

namespace scc { 

#define CreateDeclarationListType2Impl(ListType, ItemTypeName) \
  std::ostream& operator<<(std::ostream& os, const ListType& list) { \
    os << #ListType " variant " << list.hasEllipsis << ", #item " << list.items.size() << std::endl; \
    for (auto item : list.items) { \
      os << item; \
    } \
    return os; \
  }

CreateDeclarationListType2Impl(DeclarationList, Declaration)
}
