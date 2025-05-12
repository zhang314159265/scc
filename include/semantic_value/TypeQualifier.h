#pragma once

namespace scc {

enum TypeQualifier {
  CONST,
  VOLATILE,
};

static const char *type_qualifier_to_str(TypeQualifier qual) {
  switch (qual) {
  case CONST:
    return "const";
  case VOLATILE:
    return "volatile";
  default:
    assert(0);
  }
}

}
