#pragma once

#include <iostream>
#include "llvm/IR/Type.h"
#include "semantic_value/TypeCreator.h"
#include "semantic_value/TypeQualifier.h"
#include "semantic_value/TypeSpecifier.h"
#include "semantic_value/DeclarationSpecifiers.h"
#include "semantic_value/DeclarationList.h"
#include "semantic_value/DirectDeclarator.h"
#include "semantic_value/TypeQualifierList.h"
#include "semantic_value/Pointer.h"
#include "semantic_value/Declarator.h"
#include "semantic_value/InitDeclarator.h"
#include "semantic_value/InitDeclaratorList.h"
#include "semantic_value/Declaration.h"
#include "semantic_value/ExternalDeclaration.h"
#include "semantic_value/TranslationUnit.h"
#include "semantic_value/Constant.h"
#include "semantic_value/PrimaryExpression.h"
#include "semantic_value/PostfixExpression.h"
#include "semantic_value/Expression.h"
#include "semantic_value/ExpressionStatement.h"
#include "semantic_value/JumpStatement.h"
#include "semantic_value/Statement.h"
#include "semantic_value/StatementList.h"
#include "semantic_value/CompoundStatement.h"
#include "semantic_value/FunctionDefinition.h"
#include "semantic_value/AssignmentExpression.h"
#include "semantic_value/ArgumentExpressionList.h"
#include "semantic_value/Initializer.h"
#include "semantic_value/AssignmentOperator.h"
#include "semantic_value/UnaryExpression.h"
#include "semantic_value/IterationStatement.h"
#include "semantic_value/RelationalExpression.h"
#include "semantic_value/AdditiveExpression.h"
#include "semantic_value/SelectiveStatement.h"

namespace scc {


enum {
  SV_NONE,
  SV_STR,
  SV_INT,
  SV_DOUBLE,
  SV_TYPE_SPECIFIER,
  SV_TYPE_QUALIFIER,
  SV_DIRECT_DECLARATOR,
  SV_TYPE_QUALIFIER_LIST,

#define def(camel, upper, lower) SV_ ## upper,
#include "semantic_value/types.def"
#undef def

};

/*
 * SemanticValue for each token or for each parser rule.
 * Note: some of the parser rule is very specific to the grammer and is not
 * general.
 *
 * Each specific parser rule may have a class defined in
 * include/semantic_value/ for it. That class's instance is a possible
 * value of the SemanticValue class.
 */
class SemanticValue {
 public:
  explicit SemanticValue() : tag(SV_NONE) { }
  explicit SemanticValue(const char *s) : tag(SV_STR), str(s) { }
  explicit SemanticValue(const char *s, int len) : tag(SV_STR), str(s, len) { }
  explicit SemanticValue(int ival) : tag(SV_INT), ival(ival) { }
  explicit SemanticValue(double dval) : tag(SV_DOUBLE), dval(dval) { }
  // explicit SemanticValue(llvm::Type *_type) : tag(SV_TYPE), type(_type) { }
  explicit SemanticValue(TypeQualifier _qual) : tag(SV_TYPE_QUALIFIER), qual(_qual) { }
  /* implicit */ SemanticValue(TypeSpecifier _spec) : tag(SV_TYPE_SPECIFIER), spec(_spec) { }
  /* implicit */ SemanticValue(DirectDeclarator _direct_declarator) : tag(SV_DIRECT_DECLARATOR), direct_declarator(_direct_declarator) { }
  /* implicit */ SemanticValue(TypeQualifierList _type_qualifier_list) : tag(SV_TYPE_QUALIFIER_LIST), type_qualifier_list(_type_qualifier_list) { }

#define def(camel, upper, lower) \
  /* implicit */ SemanticValue(camel lower) : tag(SV_ ## upper), lower(lower) { }

#include "semantic_value/types.def"
#undef def

  friend std::ostream& operator<<(std::ostream& os, const SemanticValue &sv);
 public:
  int tag;
  union {
    int ival;
    double dval;
    // llvm::Type* type;
    TypeQualifier qual;
    TypeSpecifier spec;
  };
  std::string str; // can not put std::string inside union. So put it outside..
  DirectDeclarator direct_declarator;
  TypeQualifierList type_qualifier_list;

#define def(camel, upper, lower) \
  camel lower;

#include "semantic_value/types.def"
#undef def

};

std::ostream& operator<<(std::ostream& os, const SemanticValue &sv) {
  // std::cout << "tag is " << sv.tag << std::endl;
  switch (sv.tag) {
  case SV_STR:
    os << sv.str;
    break;
  case SV_INT:
    os << sv.ival;
    break;
  case SV_DOUBLE:
    os << sv.dval;
    break;
  #if 0
  case SV_TYPE: {
    std::string s;
    llvm::raw_string_ostream raw(s);
    sv.type->print(raw);
    os << s;
    break;
  }
  #endif
  case SV_TYPE_QUALIFIER: {
    os << type_qualifier_to_str(sv.qual); 
    break;
  }
  case SV_TYPE_SPECIFIER: {
    os << type_specifier_to_str(sv.spec);
    break;
  }
  case SV_DIRECT_DECLARATOR: {
    os << sv.direct_declarator;
    break;
  }
  case SV_TYPE_QUALIFIER_LIST: {
    os << sv.type_qualifier_list;
    break;
  }
#define def(camel, upper, lower) \
  case SV_ ## upper: { \
    os << sv.lower; \
    break; \
  }
#include "semantic_value/types.def"
#undef def

  default:
    assert(false);
  }
  return os;
}

}
