#pragma once

// Wrapper type
#define CreateWrapperType(ParentType, ChildType, item) \
  class ParentType { \
   public: \
    ParentType() { } \
    ParentType(const ChildType& item) : item(item) { } \
   public: \
    ChildType item; \
    friend std::ostream& operator<<(std::ostream& os, const ParentType& parent) { \
      os << #ParentType << std::endl; \
      os << parent.item; \
      return os; \
    } \
  };


// List type

#define CreateListTypeDecl(ItemTypeName) \
  CreateListType2Decl(ItemTypeName ## List, ItemTypeName)

#define CreateListTypeImpl(ItemTypeName) \
  CreateListType2Impl(ItemTypeName ## List, ItemTypeName)

#define CreateListType2(ListType, ItemTypeName) \
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
    friend std::ostream& operator<<(std::ostream& os, const ListType& list) { \
      os << #ListType " " << list.items.size() << std::endl; \
      for (auto item : list.items) { \
        os << item; \
      } \
      return os; \
    }; \
  };

#define CreateListType2Decl(ListType, ItemTypeName) \
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
    friend std::ostream& operator<<(std::ostream& os, const ListType& list); \
  };

#define CreateListType2Impl(ListType, ItemTypeName) \
  std::ostream& operator<<(std::ostream& os, const ListType& list) { \
    os << #ListType " " << list.items.size() << std::endl; \
    for (auto item : list.items) { \
      os << item; \
    } \
    return os; \
  }

// Left associative expression chain
#define CreateLeftAssocExprChain(ParentType, SubType, OpType) \
  class ParentType { \
   public: \
    ParentType() {} \
    ParentType(const SubType& firstitem) { \
      this->firstitem = firstitem; \
    } \
    \
    ParentType& addItem(OpType op, const SubType& item) { \
      pairlist.emplace_back(op, item); \
      return * this; \
    } \
   public: \
    SubType firstitem; \
    std::vector<std::pair<OpType, SubType>> pairlist; \
    friend std::ostream& operator<<(std::ostream& os, const ParentType& parent) { \
      os << #ParentType " " << parent.pairlist.size() << std::endl; \
      os << "firstitem: " << parent.firstitem << std::endl; \
      for (auto &pair : parent.pairlist) { \
        os << pair.first << " " << pair.second; \
      } \
      return os; \
    }; \
  };


// Compose type
#define CreateComposeType2Decl(ComposeType, Type1, item1, Type2, item2) \
  class ComposeType { \
   public: \
    ComposeType() { } \
    ComposeType(const Type1& item1, const Type2 &item2) : item1(item1), item2(item2) { } \
   public: \
    Type1 item1; \
    Type2 item2; \
    friend std::ostream& operator<<(std::ostream& os, const ComposeType& parent); \
  };

#define CreateComposeType2Impl(ComposeType, Type1, item1, Type2, item2) \
  std::ostream& operator<<(std::ostream& os, const ComposeType& parent) { \
    os << #ComposeType << std::endl; \
    os << parent.item1 << std::endl; \
    os << parent.item2 << std::endl; \
    return os; \
  }

#define CreateComposeType2(ComposeType, Type1, item1, Type2, item2) \
  class ComposeType { \
   public: \
    ComposeType() { } \
    ComposeType(const Type1& item1, const Type2 &item2) : item1(item1), item2(item2) { } \
   public: \
    Type1 item1; \
    Type2 item2; \
    friend std::ostream& operator<<(std::ostream& os, const ComposeType& parent) { \
      os << #ComposeType << std::endl; \
      os << parent.item1; \
      os << parent.item2; \
      return os; \
    } \
  };

#define CreateComposeType3(ComposeType, Type1, item1, Type2, item2, Type3, item3) \
  class ComposeType { \
   public: \
    ComposeType() { } \
    ComposeType(const Type1& item1, const Type2 &item2, const Type3 &item3) : item1(item1), item2(item2), item3(item3) { } \
   public: \
    Type1 item1; \
    Type2 item2; \
    Type3 item3; \
    friend std::ostream& operator<<(std::ostream& os, const ComposeType& parent) { \
      os << #ComposeType << std::endl; \
      os << parent.item1; \
      os << parent.item2; \
      os << parent.item3; \
      return os; \
    } \
  };



// union type
#define CreateUnionType2Decl(ResultType, Type1, val1, Type2, val2) \
enum { \
  ResultType ## _ ## None, \
  ResultType ## _ ## Type1, \
  ResultType ## _ ## Type2, \
}; \
\
class ResultType { \
 public: \
  ResultType() : tag(ResultType ## _ ## None) {} \
  ResultType(const Type1& val1); \
  ResultType(const Type2& val2); \
  static const char *tag2str(int tag) { \
    switch (tag) { \
    case ResultType ## _ ## Type1: \
      return #ResultType "_" # Type1; \
    case ResultType ## _ ## Type2: \
      return #ResultType "_" # Type2; \
    default: \
      assert(0); \
    } \
  } \
 public: \
  int tag; \
  Type1 val1; \
  Type2 val2; \
  friend std::ostream& operator<<(std::ostream& os, const ResultType& parent); \
};

#define CreateUnionType3Decl(ResultType, Type1, val1, Type2, val2, Type3, val3) \
enum { \
  ResultType ## _ ## None, \
  ResultType ## _ ## Type1, \
  ResultType ## _ ## Type2, \
  ResultType ## _ ## Type3, \
}; \
\
class ResultType { \
 public: \
  ResultType() : tag(ResultType ## _ ## None) {} \
  ResultType(const Type1& val1); \
  ResultType(const Type2& val2); \
  ResultType(const Type3& val3); \
  static const char *tag2str(int tag) { \
    switch (tag) { \
    case ResultType ## _ ## Type1: \
      return #ResultType "_" # Type1; \
    case ResultType ## _ ## Type2: \
      return #ResultType "_" # Type2; \
    case ResultType ## _ ## Type3: \
      return #ResultType "_" # Type3; \
    default: \
      assert(0); \
    } \
  } \
 public: \
  int tag; \
  Type1 val1; \
  Type2 val2; \
  Type3 val3; \
  friend std::ostream& operator<<(std::ostream& os, const ResultType& parent); \
};



#define CreateUnionType2Impl(ResultType, Type1, val1, Type2, val2) \
ResultType::ResultType(const Type1& val1) : tag(ResultType ## _ ## Type1), val1(val1) {} \
ResultType::ResultType(const Type2& val2) : tag(ResultType ## _ ## Type2), val2(val2) {} \
std::ostream& operator<<(std::ostream& os, const ResultType& parent) { \
  os << #ResultType << "(tag " << ResultType::tag2str(parent.tag) << ")" << std::endl; \
  switch (parent.tag) { \
  case ResultType ## _ ## Type1: \
    os << parent.val1; \
    break; \
  case ResultType ## _ ## Type2: \
    os << parent.val2; \
    break; \
  default: \
    assert(0); \
    break; \
  } \
  return os; \
}

#define CreateUnionType3Impl(ResultType, Type1, val1, Type2, val2, Type3, val3) \
ResultType::ResultType(const Type1& val1) : tag(ResultType ## _ ## Type1), val1(val1) {} \
ResultType::ResultType(const Type2& val2) : tag(ResultType ## _ ## Type2), val2(val2) {} \
ResultType::ResultType(const Type3& val3) : tag(ResultType ## _ ## Type3), val3(val3) {} \
std::ostream& operator<<(std::ostream& os, const ResultType& parent) { \
  os << #ResultType << "(tag " << ResultType::tag2str(parent.tag) << ")" << std::endl; \
  switch (parent.tag) { \
  case ResultType ## _ ## Type1: \
    os << parent.val1; \
    break; \
  case ResultType ## _ ## Type2: \
    os << parent.val2; \
    break; \
  case ResultType ## _ ## Type3: \
    os << parent.val3; \
    break; \
  default: \
    assert(0); \
    break; \
  } \
  return os; \
}

#define CreateUnionType2(ResultType, Type1, val1, Type2, val2) \
enum { \
  ResultType ## _ ## None, \
  ResultType ## _ ## Type1, \
  ResultType ## _ ## Type2, \
}; \
\
class ResultType { \
 public: \
  ResultType() : tag(ResultType ## _ ## None) {} \
  ResultType(const Type1& val1) : tag(ResultType ## _ ## Type1), val1(val1) {} \
  ResultType(const Type2& val2) : tag(ResultType ## _ ## Type2), val2(val2) {} \
  static const char *tag2str(int tag) { \
    switch (tag) { \
    case ResultType ## _ ## Type1: \
      return #ResultType "_" # Type1; \
    case ResultType ## _ ## Type2: \
      return #ResultType "_" # Type2; \
    default: \
      std::cout << "Invalid tag " << tag << std::endl; \
      assert(0); \
    } \
  } \
 public: \
  int tag; \
  Type1 val1; \
  Type2 val2; \
  friend std::ostream& operator<<(std::ostream& os, const ResultType& parent) { \
    os << #ResultType << "(tag " << ResultType::tag2str(parent.tag) << ")" << std::endl; \
    switch (parent.tag) { \
    case ResultType ## _ ## Type1: \
      os << parent.val1; \
      break; \
    case ResultType ## _ ## Type2: \
      os << parent.val2; \
      break; \
    default: \
      assert(0); \
      break; \
    } \
    return os; \
  } \
};
