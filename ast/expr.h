#pragma once

#include <memory>
#include <fmt/format.h>
#include "ast/node.h"
#include "label.h"
#include "type.h"
#include "symtab.h"

namespace ast {

// expression subclasses
class Expr;
class AssignExpr;
class BinOpExpr;

// baseclass of all expressions
class ExprBase : public Node {
 public:
  explicit ExprBase(Type* type) : type_(type) { }

  // generate rvalue
  virtual std::unique_ptr<ExprBase> gen(Emitter* emitter) {
    throw std::runtime_error(
      fmt::format("Please override gen method in subclass: {}", typeid(*this).name()));
  }

  // generate lvalue for ArrayReference
  // Only some expression like ArrayAccess need use emitter to gen
  // code that calculates offset.
  virtual std::string getAddrStr(Emitter* emitter) {
    throw std::runtime_error(
      fmt::format("Please override getAddrStr method in subclass if the subclass can be an address in IR: {}", typeid(*this).name()));
  }

  virtual void jumping(Emitter* emitter, Label* trueLabel, Label* falseLabel) {
    throw std::runtime_error(
      fmt::format("Please override jumping method in subclass: {}", typeid(*this).name()));
  }

  virtual std::unique_ptr<ExprBase> clone() {
    throw std::runtime_error(
      fmt::format("Please override clone method in subclass: {}", typeid(*this).name()));
  }

  Type* type() {
    return type_;
  }
 protected:
  Type* type_;
};

class AssignExpr : public ExprBase {
 public:
  explicit AssignExpr(ExprBase* lhs, ExprBase* rhs) : ExprBase(nullptr), lhs_(lhs), rhs_(rhs) {
    // only support equal type for now
    assert(typeEq(lhs->type(), rhs->type()));
    type_ = rhs->type();
  }

  void dump(int depth=0) override {
    Node::indent(depth);
    std::cout << "ASSIGN" << std::endl;
    lhs_->dump(depth + 1);
    rhs_->dump(depth + 1);
  }

  std::unique_ptr<ExprBase> gen(Emitter* emitter) override;
 private:
  std::unique_ptr<ExprBase> lhs_, rhs_;
};

// The expression node representing a list of assignment_expression
// separated by commas
class CommaExpr : public ExprBase, public NodeList<AssignExpr> {
 public:
	using ElemType = AssignExpr;

  explicit CommaExpr() : ExprBase(nullptr) { }

  NodeList<AssignExpr>& addElem(AssignExpr* elemPtr) override {
    type_ = elemPtr->type();
    return NodeList<AssignExpr>::addElem(elemPtr);
  }

	void dump(int depth=0) override {
		dumpWithTag("COMMA EXPR", depth);
	}
  std::unique_ptr<ExprBase> gen(Emitter* emitter) override;
  void jumping(Emitter* emitter, Label* trueLabel, Label* falseLabel) override;
};

class BinOpExpr : public ExprBase {
 public:
  explicit BinOpExpr(const std::string& opstr, ExprBase* lhs, ExprBase* rhs)
		: ExprBase(nullptr), opstr_(opstr), lhs_(lhs), rhs_(rhs) {
    assert(typeEq(lhs->type(), rhs->type()));
    type_ = lhs->type();
  }

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
  explicit IntConst(int ival) : ExprBase(&Type::INT), ival_(ival) { }
  void dump(int depth=0) override {
    Node::indent(depth);
    std::cout << "INT_CONST " << ival_ << std::endl;
  }
  std::unique_ptr<ExprBase> gen(Emitter* emitter) override {
    return std::make_unique<IntConst>(ival_);
  }
  std::string getAddrStr(Emitter* emitter) override {
    return fmt::format("{}", ival_);
  }
  int ival() const { return ival_; }
 private:
  int ival_;
};

class Temp : public ExprBase {
 public:
  explicit Temp(int temp_id, Type* type) : ExprBase(type), temp_id_(temp_id) {
  }

  std::string getAddrStr(Emitter* emitter) override {
    return fmt::format("t{}", temp_id_);
  }
 private:
  int temp_id_;
};

class Id : public ExprBase {
 public:
  explicit Id(const std::string& str) : ExprBase(Symtab::cur()->getType(str)), str_(str) { }
  void dump(int depth=0) override {
    Node::indent(depth);
    std::cout << "ID " << str_ << std::endl;
  }
  void jumping(Emitter* emitter, Label* trueLabel, Label* falseLabel) override;
  std::unique_ptr<ExprBase> gen(Emitter* emitter) override {
    return std::make_unique<Id>(str_);
  }
  std::string getAddrStr(Emitter* emitter) override {
    return str_;
  }
  std::unique_ptr<ExprBase> clone() override {
    return std::make_unique<Id>(str_);
  }
 private:
  std::string str_;
};

class IncDec : public ExprBase {
 public:
  explicit IncDec(ExprBase* child, bool is_inc, bool is_pre)
    : ExprBase(child->type()), child_(child), is_inc_(is_inc), is_pre_(is_pre) { }
  void dump(int depth=0) override {
    Node::indent(depth);
    std::cout << (is_pre_ ? "PRE" : "POST") << " " << (is_inc_ ? "INC" : "DEC") << std::endl;
    child_->dump(depth + 1);
  }

  std::unique_ptr<ExprBase> gen(Emitter* emitter) override;
 private:
  std::string opstr() const {
    return is_inc_ ? "+" : "-";
  }
  std::unique_ptr<ExprBase> child_;
  bool is_inc_;
  bool is_pre_;
};

class ArrayAccess : public ExprBase {
 public:
  explicit ArrayAccess(ExprBase* array, ExprBase* index) : ExprBase(nullptr), array_(array), index_(index) {
    auto arrayTy = dynamic_cast<ArrayType*>(array->type());
    assert(arrayTy);
    type_ = arrayTy->of();
  }

  void dump(int depth=0) override {
    Node::indent(depth);
    std::cout << "ARRAY ACCESS" << std::endl;
    array_->dump(depth + 1);
    index_->dump(depth + 1);
  }
  
  // create byte offset
  std::unique_ptr<Temp> createOffsetExpr(Emitter* emitter);
  std::unique_ptr<ExprBase> gen(Emitter* emitter) override;
  std::string getAddrStr(Emitter* emitter) override;
 private:
  std::unique_ptr<ExprBase> array_, index_;
};

};
