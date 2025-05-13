#pragma once
#include "semantic_value/Constant.h"

namespace scc {

enum {
  PrimaryExpression_None,
  PrimaryExpression_IDENTIFIER,
  PrimaryExpression_STR,
  PrimaryExpression_constant,
};

class PrimaryExpression {
 public:
  PrimaryExpression() : tag(PrimaryExpression_None) {}
  PrimaryExpression(const std::string& str, int tag) : tag(tag), str(str) {}
  PrimaryExpression(const Constant& constant) : tag(PrimaryExpression_constant), constant(constant) {}

  static const char *tag2str(int tag) {
    switch (tag) {
    case PrimaryExpression_IDENTIFIER:
      return "PrimaryExpression_IDENTIFIER";
    case PrimaryExpression_STR:
      return "PrimaryExpression_STR";
    case PrimaryExpression_constant:
      return "PrimaryExpression_constant";
    default:
      assert(0);
    }
  }
 public:
  int tag;
  std::string str;
  Constant constant;

  friend std::ostream& operator<<(std::ostream& os, const PrimaryExpression& parent);

};

}
