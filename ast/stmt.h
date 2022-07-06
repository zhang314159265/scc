#pragma once

#include "ast/expr.h"

namespace ast {

// statement subclasses
class StmtList;
class ExprStmt;

// baseclass for all statements
class Stmt : public Node {
 public:
  virtual void emit(Emitter* emitter) {
    std::cout << "emit.. " << typeid(*this).name() << std::endl;
  }
};

class StmtList : public Stmt, public NodeList<Stmt> {
 public:
 	using ElemType = Stmt;

  void dump(int depth=0) override {
		dumpWithTag("STMT_LIST", depth);
	}
  void emit(Emitter* emitter) override {
    for (auto& itemptr : list_) {
      itemptr->emit(emitter);
    }
  }
};

class ExprStmt : public Stmt {
 public:
  explicit ExprStmt(ExprBase* nested) : expr_(nested) {
	}

	void dump(int depth=0) override {
		Node::indent(depth);
		std::cout << "EXPR_STMT";
		if (expr_) {
		  std::cout << std::endl;
			expr_->dump(depth + 1);
		} else {
		  std::cout << " (null)" << std::endl;
		}
	}
  void emit(Emitter* emitter) override {
    if (expr_) {
      /* ignore */ expr_->gen(emitter);
    }
  }
 private:
  std::unique_ptr<ExprBase> expr_; // can be nullptr
};

}
