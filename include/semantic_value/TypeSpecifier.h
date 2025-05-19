#pragma once

namespace scc {

enum TypeSpecifier {
  CHAR,
  INT,
  FLOAT,
  DOUBLE,
  VOID,
};

static const char *type_specifier_to_str(TypeSpecifier spec) {
  switch (spec) {
  case CHAR:
    return "char";
  case INT:
    return "int";
  case FLOAT:
    return "float";
  case DOUBLE:
    return "double";
  case VOID:
    return "void";
  default:
    assert(0);
  }
}

}
