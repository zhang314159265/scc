#pragma once

#include "ast/expr.h"

/*
 * Emit 3 address code
 */
class Emitter {
 public:
  void emit(const std::string& instr) {
    std::cout << "  " << instr << std::endl;
  }

  std::unique_ptr<ast::Temp> createTemp() {
    return std::make_unique<ast::Temp>(++next_temp_id_);
  }
 private:
  int next_temp_id_ = 0;
};
