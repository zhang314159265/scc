#pragma once

#include <iostream>
#include <fstream>
#include "ast/expr.h"
#include "label.h"
#include "type.h"

/*
 * Emit 3 address code
 */
class Emitter {
 public:
  explicit Emitter(const std::string& path) {
    if (path.size()) {
      ofs_ = std::make_unique<std::ofstream>(path, std::ofstream::out);
    }
  }

  void emit(const std::string& instr) {
    getouts() << "  " << instr << std::endl;
  }

  void emitLabel(Label* label) {
    getouts() << label->use() << ":" << std::endl;
  }

  void emitJump(Label* label) {
    emit(fmt::format("goto {}", label->use()));
  }

  void emitCondJump(const std::string& testStr, Label* label, bool negate) {
    emit(fmt::format("if{} {} goto {}",
      negate ? "false" : "",
      testStr,
      label->use()));
  }

  std::unique_ptr<ast::Temp> createTemp(Type* type) {
    return std::make_unique<ast::Temp>(++next_temp_id_, type);
  }

  std::ostream& getouts() {
    if (ofs_) {
      return *ofs_;
    } else {
      return std::cout;
    }
  }

  void flush() {
    std::flush(getouts());
  }
 private:
  int next_temp_id_ = 0;
  std::unique_ptr<std::ofstream> ofs_;
};
