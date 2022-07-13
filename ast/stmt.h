#pragma once

#include "ast/expr.h"
#include "label.h"
#include "emitter.h"

namespace ast {

// statement subclasses
class StmtList;
class ExprStmt;
class While;

// TODO rename Stmt class to StmtBase
#define StmtBase Stmt

// baseclass for all statements
class Stmt : public Node {
 public:
  virtual void emit(Emitter* emitter, Label* next) {
    throw std::runtime_error(
      fmt::format("Please override emit method in subclass: {}", typeid(*this).name()));
  }
};

class StmtList : public Stmt, public NodeList<Stmt> {
 public:
 	using ElemType = Stmt;

  void dump(int depth=0) override {
		dumpWithTag("STMT_LIST", depth);
	}
  void emit(Emitter* emitter, Label* next) override {
    int sz = list_.size();
    Label itemnext;
    for (int i = 0; i < sz; ++i) {
      auto& itemptr = list_[i];
      Label* labelptr = nullptr;
      if (i == sz - 1) {
        labelptr = next;
      } else {
        labelptr = &itemnext;
      }
      itemptr->emit(emitter, labelptr);
      if (labelptr->used() && i != sz - 1) {
        // emit the label unless it's the last item for which the label will
        // be emitted by the caller.
        labelptr->emit_if_used(emitter);
        itemnext = Label(); // create a new label for the next iteration
      }
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
  void emit(Emitter* emitter, Label* /* next */) override {
    if (expr_) {
      /* ignore */ expr_->gen(emitter);
    }
  }
 private:
  std::unique_ptr<ExprBase> expr_; // can be nullptr
};

class While : public Stmt {
 public:
  explicit While(ExprBase* expr, Stmt* stmt) : expr_(expr), stmt_(stmt) { }
  void dump(int depth=0) override {
    Node::indent(depth);
    std::cout << "WHILE" << std::endl;
    expr_->dump(depth + 1);
    stmt_->dump(depth + 1);
  }

  void emit(Emitter* emitter, Label* next) override {
    Label loopbegin;
    emitter->emitLabel(&loopbegin);
    expr_->jumping(emitter, nullptr, next);
    stmt_->emit(emitter, &loopbegin);
    emitter->emitJump(&loopbegin);
  }
 private:
  std::unique_ptr<ExprBase> expr_;
  std::unique_ptr<Stmt> stmt_;
};

class For : public Stmt {
 public:
  explicit For(ExprBase* init, ExprBase* cond, ExprBase* post, StmtBase* body)
    : init_(init), cond_(cond), post_(post), body_(body) { }

  void dump(int depth=0) override {
    Node::indent(depth);
    std::cout << "FOR" << std::endl;
    if (init_) {
      init_->dump(depth + 1);
    } else {
      Node::indent(depth + 1);
      std::cout << "NO INIT" << std::endl;
    }
    if (cond_) {
      cond_->dump(depth + 1);
    } else {
      Node::indent(depth + 1);
      std::cout << "NO COND" << std::endl;
    }
    if (post_) {
      post_->dump(depth + 1);
    } else {
      Node::indent(depth + 1);
      std::cout << "NO POST" << std::endl;
    }
    body_->dump(depth + 1);
  }

  void emit(Emitter* emitter, Label* next) override {
    if (init_) {
      init_->gen(emitter);
    }
    Label loopbegin;
    emitter->emitLabel(&loopbegin);
    if (cond_) {
      cond_->jumping(emitter, nullptr, next);
    } else {
      // always fall thru if cond_ is missing
    }

    body_->emit(emitter, next);
    if (post_) {
      post_->gen(emitter);
    }
    // jump to loop begin
    emitter->emitJump(&loopbegin);
  }
 private:
  std::unique_ptr<ExprBase> init_; // can be nullptr
  std::unique_ptr<ExprBase> cond_; // can be nullptr
  std::unique_ptr<ExprBase> post_; // can be nullptr
  std::unique_ptr<StmtBase> body_;
};

}
