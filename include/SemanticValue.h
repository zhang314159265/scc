#pragma once

namespace scc {

enum {
  SV_NONE,
  SV_STR,
  SV_INT,
};

/*
 * SemanticValue for each token or for each parser rule.
 */
class SemanticValue {
 public:
  explicit SemanticValue() : tag(SV_NONE) { }
  explicit SemanticValue(const char *s) : tag(SV_STR), str(s) { }
  explicit SemanticValue(const char *s, int len) : tag(SV_STR), str(s, len) { }
  explicit SemanticValue(int ival) : tag(SV_INT), ival(ival) { }

  friend std::ostream& operator<<(std::ostream& os, const SemanticValue &sv);
 private:
  int tag;
  union {
    int ival;
  };
  std::string str; // can not put std::string inside union. So put it outside..
};

std::ostream& operator<<(std::ostream& os, const SemanticValue &sv) {
  switch (sv.tag) {
  case SV_STR:
    os << sv.str;
    break;
  case SV_INT:
    os << sv.ival;
    break;
  default:
    assert(false);
  }
  return os;
}

}
