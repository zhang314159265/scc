#pragma once

#include <memory>
#include <fmt/format.h>
#include "ast/node.h"

namespace ast {

// expression subclasses
class Expr;
class AssignExpr;
class BinOpExpr;

// baseclass of all expressions
class ExprBase : public Node {
 public:
  virtual std::unique_ptr<ExprBase> gen(Emitter* emitter) {
    throw std::runtime_error(
      fmt::format("Please override gen method in subclass: {}", typeid(*this).name()));
  }

  virtual std::string getAddrStr() {
    throw std::runtime_error(
      fmt::format("Please override getAddrStr method in subclass if the subclass can be an address in IR: {}", typeid(*this).name()));
  }
};

// The expression node representing a list of assignment_expression
// separated by commas
class CommaExpr : public ExprBase, public NodeList<AssignExpr> {
 public:
	using ElemType = AssignExpr;

	void dump(int depth=0) override {
		dumpWithTag("EXPR", depth);
	}
  std::unique_ptr<ExprBase> gen(Emitter* emitter) override;
};

class AssignExpr : public ExprBase {
};

class BinOpExpr : public ExprBase {
 public:
  explicit BinOpExpr(const std::string& opstr, ExprBase* lhs, ExprBase* rhs)
		: opstr_(opstr), lhs_(lhs), rhs_(rhs) { }

	void dump(int depth=0) override {
		Node::indent(depth);
		std::cout << "BINOP " << opstr_ << std::endl;
		lhs_->dump(depth + 1);
		rhs_->dump(depth + 1);
	}
  std::unique_ptr<ExprBase> gen(Emitter* emitter) override;
 private:
  std::unique_ptr<ExprBase> lhs_;
	std::unique_ptr<ExprBase> rhs_;
	std::string opstr_;
};

class IntConst : public ExprBase {
 public:
  explicit IntConst(int ival) : ival_(ival) { }
  void dump(int depth=0) override {
    Node::indent(depth);
    std::cout << "INT_CONST " << ival_ << std::endl;
  }
  std::unique_ptr<ExprBase> gen(Emitter* emitter) override {
    return std::make_unique<IntConst>(ival_);
  }
  std::string getAddrStr() override {
    return fmt::format("{}", ival_);
  }
 private:
  int ival_;
};

class Temp : public ExprBase {
 public:
  explicit Temp(int temp_id) : temp_id_(temp_id) {
  }

  std::string getAddrStr() override {
    return fmt::format("t{}", temp_id_);
  }
 private:
  int temp_id_;
};

};
