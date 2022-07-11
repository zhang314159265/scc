#pragma once

#include "ast/expr.h"
#include "label.h"

/*
 * Emit 3 address code
 */
class Emitter {
 public:
  void emit(const std::string& instr) {
    std::cout << "  " << instr << std::endl;
  }

  void emitLabel(Label* label) {
    std::cout << label->use() << ":" << std::endl;
  }

  void emitJump(Label* label) {
    emit(fmt::format("goto {}", label->use()));
  }

  void emitCondJump(const std::string& testStr, Label* label, bool negate) {
    emit(fmt::format("if{} {} goto {}",
      negate ? "false" : "true",
      testStr,
      label->use()));
  }

  std::unique_ptr<ast::Temp> createTemp() {
    return std::make_unique<ast::Temp>(++next_temp_id_);
  }
 private:
  int next_temp_id_ = 0;
};
