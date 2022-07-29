#pragma once

#include <string>

class Util {
 public:
  static void replaceAll(std::string& mainstr, const std::string& needle, const std::string& replacement) {
    assert(needle.size() > 0);
    int pos = 0;
    while ((pos = mainstr.find(needle, pos)) != std::string::npos) {
      mainstr.replace(pos, needle.size(), replacement);
      pos += replacement.size();
    }
  }
};
