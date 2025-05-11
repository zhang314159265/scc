#define def(name, val) name = val,
enum {
#include "token.def"
};
#undef def

// not thread safe
const char *tok_to_str(int tok) {
  switch (tok) {
  #define def(name, val) case val: return #name;
  #include "token.def"
  #undef def
  }

  if (tok < 0 || tok >= 256) {
    return "unknown";
  }

  // try to interprete as a printable character
  static char buf[2] = {0};
  buf[0] = tok;
  return buf;
}

