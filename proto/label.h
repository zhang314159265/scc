#pragma once

#include <string>
#include <fmt/format.h>

class Emitter;

/*
 * A null Label ptr indicates fallthru.
 * A non null Label return false for used() if the label is not a target of any
 * jumps.
 */
class Label {
 public:
  Label(): no_(-1) { }

  bool used() {
    return no_ >= 0;
  }

  std::string use() {
    if (no_ < 0) {
      no_ = next_no_++;
    }
    return fmt::format("L{}", no_);
  }

  void emit_if_used(Emitter* emitter);
  void emit(Emitter* emitter);
 private:
  int no_;
  static int next_no_;
};
