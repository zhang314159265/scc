#pragma once
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"
#include "semantic_value/TranslationUnit.h"
#include <iostream>

namespace scc {

struct LoweredDeclaration {
  std::string name;
  llvm::Type *type;
};

void to_llir(TranslationUnit& tu, llvm::Module &M);
void to_llir(ExternalDeclaration& ed, llvm::Module &M);
LoweredDeclaration to_llir(Declaration& d, llvm::Module &M);
llvm::Type *to_llir(DeclarationSpecifiers &ds, llvm::Module& M);
llvm::Type *to_llir(TypeSpecifier &ts, llvm::Module &M);
LoweredDeclaration to_llir(InitDeclarator &id, llvm::Type* type, llvm::Module &M);
LoweredDeclaration to_llir(Declarator &de, llvm::Type* type, llvm::Module &M);
llvm::Type *to_llir(Pointer &pointer, llvm::Type *type);
LoweredDeclaration to_llir(DirectDeclarator &dd, llvm::Type *type, llvm::Module &M);
void to_llir(CompoundStatement &cs, llvm::Module &M, llvm::IRBuilder<> &B);
void to_llir(Statement &st, llvm::Module &M, llvm::IRBuilder<> &B);
void to_llir(ExpressionStatement &es, llvm::Module &M, llvm::IRBuilder<> &B);
llvm::Value *to_llir(Expression &e, llvm::Module &M, llvm::IRBuilder<> &B);
llvm::Value *to_llir(AssignmentExpression &ae, llvm::Module &M, llvm::IRBuilder<> &B);
llvm::Value *to_llir(PostfixExpression &ce, llvm::Module &M, llvm::IRBuilder<> &B);
llvm::Value *to_llir(PrimaryExpression &ce, llvm::Module &M, llvm::IRBuilder<> &B);
void to_llir(JumpStatement &js, llvm::Module &M, llvm::IRBuilder<> &B);
llvm::Value *to_llir(Constant &con, llvm::Module &M, llvm::IRBuilder<> &B);

void to_llir(TranslationUnit& tu, llvm::Module &M) {
  for (auto &external_declaration : tu.items) {
    to_llir(external_declaration, M);
  }
}

void to_llir(ExternalDeclaration& ed, llvm::Module &M) {
  switch (ed.tag) {
  case ExternalDeclaration_FunctionDefinition: {
    auto &def = ed.function_definition;
    llvm::Type *type = to_llir(def.declaration_specifiers, M);
    LoweredDeclaration ld = to_llir(def.declarator, type, M);
    assert(ld.name.size() > 0);
    assert(ld.type->isFunctionTy());

    llvm::Function *F = llvm::Function::Create(
      (llvm::FunctionType *) ld.type,
      llvm::Function::ExternalLinkage,
      ld.name,
      M);

    llvm::LLVMContext &C = M.getContext();
    llvm::BasicBlock *BB = llvm::BasicBlock::Create(
      C,
      "entry",
      F
    );
    llvm::IRBuilder<> B(C);
    B.SetInsertPoint(BB);

    to_llir(def.compound_statement, M, B);
    break;
  }
  case ExternalDeclaration_Declaration: {
    auto loweredDeclaration = to_llir(ed.declaration, M);
    auto name = loweredDeclaration.name;
    llvm::Type *type = loweredDeclaration.type;
    if (type->isFunctionTy()) {
      assert(name.size() > 0); 
      M.getOrInsertFunction(
        name,
        (llvm::FunctionType*) type
      );
    }
    break;
  }
  default:
    assert(0);
  }
}

LoweredDeclaration to_llir(Declaration& d, llvm::Module &M) {
  llvm::Type *type = to_llir(d.declaration_specifiers, M);

  if (d.init_declarator_list.items.size() == 0) {
    return {std::string(), type};
  }

  assert(d.init_declarator_list.items.size() == 1);
  for (auto &init_declarator : d.init_declarator_list.items) {
    return to_llir(init_declarator, type, M);
  }
  assert(0);
}

llvm::Type *to_llir(DeclarationSpecifiers &ds, llvm::Module& M) {
  assert(ds.speclist.size() == 1);

  llvm::Type *type = to_llir(ds.speclist[0], M);
  #if 0 // ignore qualifier
  for (TypeQualifier qual : ds.quallist) {
    switch (qual) {
    case CONST:
      assert(false);
    default:
      assert(false);
    }
  }
  #endif
  return type;
}

llvm::Type *to_llir(TypeSpecifier &ts, llvm::Module &M) {
  llvm::LLVMContext &C = M.getContext();
  switch (ts) {
  case INT:
    return llvm::Type::getInt32Ty(C);
  case CHAR:
    return llvm::Type::getInt8Ty(C);
  case VOID:
    return llvm::Type::getVoidTy(C);
  default:
    assert(0);
  }
  return NULL;
}

LoweredDeclaration to_llir(InitDeclarator &id, llvm::Type* type, llvm::Module &M) {
  return to_llir(id.declarator, type, M);
}

LoweredDeclaration to_llir(Declarator &de, llvm::Type* type, llvm::Module &M) {
  type = to_llir(de.pointer, type);
  return to_llir(de.direct_declarator, type, M);
}

llvm::Type *to_llir(Pointer &pointer, llvm::Type *type) {
  for (TypeQualifierList quallist : pointer.quallists) {
    // ignore qualifiers
    type = llvm::PointerType::get(type, 0);
  }
  return type;
}

LoweredDeclaration to_llir(DirectDeclarator &dd, llvm::Type *type, llvm::Module &M) {
  const std::string &name = dd.identifier;
  std::vector<DeclarationList> decl_list_list = dd.decl_list_list;

  if (decl_list_list.size() == 1) {
    // function declaration
    DeclarationList &decl_list = decl_list_list[0];
    std::vector<llvm::Type*> argTypes;
    for (Declaration &decl : decl_list.items) {
      LoweredDeclaration ld = to_llir(decl, M);
      // ld.name does not matter
      argTypes.push_back(ld.type);
    }

    if (argTypes.size() == 1 && argTypes[0]->isVoidTy()) {
      argTypes.clear(); 
    }

    // create function type
    llvm::FunctionType *funcType = llvm::FunctionType::get(
      type,
      argTypes,
      false
    );
    return {name, funcType};
  } else if (decl_list_list.size() == 0) {
    return {name, type};
  }

  assert(0);
}

void to_llir(CompoundStatement &cs, llvm::Module&M, llvm::IRBuilder<> &B) {
  assert(cs.declaration_list.items.size() == 0);
  for (auto &statement : cs.statement_list.items) {
    to_llir(statement, M, B);
  }
}

void to_llir(Statement &st, llvm::Module& M, llvm::IRBuilder<> &B) {
  switch (st.tag) {
  case Statement_ExpressionStatement:
    to_llir(st.expression_statement, M, B);
    break;
  case Statement_JumpStatement:
    to_llir(st.jump_statement, M, B);
    break;
  default:
    assert(0);
  }
}

void to_llir(JumpStatement &js, llvm::Module &M, llvm::IRBuilder<> &B) {
  switch (js.tag) {
  case JumpStatement_RETURN: {
    llvm::Value *val = to_llir(js.expression, M, B);  
    B.CreateRet(val);
    break;
  }
  default:
    assert(0);
  }
}

void to_llir(ExpressionStatement &es, llvm::Module &M, llvm::IRBuilder<> &B) {
  to_llir(es.expression, M, B);
}

llvm::Value *to_llir(Expression &e, llvm::Module& M, llvm::IRBuilder<> &B) {
  // return llvm::Value* for the last assignment expression
  llvm::Value *value = NULL;
  assert(e.items.size() > 0);
  for (auto &assignmentExpression : e.items) {
    value = to_llir(assignmentExpression, M, B);
  }
  return value;
}

llvm::Value *to_llir(AssignmentExpression &ae, llvm::Module &M, llvm::IRBuilder<> &B) {
  return to_llir(ae.conditional_expression, M, B);
}

llvm::Value *to_llir(PostfixExpression &pe, llvm::Module &M, llvm::IRBuilder<> &B) {
  llvm::Value *primary = to_llir(pe.primary_expression, M, B);

  if (pe.postfixList.size() == 0) {
    return primary;
  }

  assert(pe.postfixList.size() == 1);
  auto &postfix = pe.postfixList[0];
  assert(postfix.tag == PostfixExpression_CALL);

  std::vector<llvm::Value *> args;
  for (auto& assignmentExpr : postfix.argument_expression_list_ptr->items) {
    args.push_back(to_llir(assignmentExpr, M, B));
  }

  llvm::Type *primary_type = primary->getType();
  auto ptr_type = llvm::cast<llvm::PointerType>(primary_type);
  auto function = llvm::cast<llvm::Function>(primary);
  auto functionType = function->getFunctionType();
  llvm::FunctionCallee callee(functionType, primary);
  return B.CreateCall(callee, args);
}

llvm::Value *to_llir(PrimaryExpression &pe, llvm::Module &M, llvm::IRBuilder<> &B) {
  switch (pe.tag) {
  case PrimaryExpression_IDENTIFIER: {
    // TODO handle local variable
    llvm::GlobalValue *gv = M.getNamedValue(pe.str);
    assert(gv);
    return gv;
  }
  case PrimaryExpression_STR: {
    return B.CreateGlobalString(pe.str);
  }
  case PrimaryExpression_constant:
    return to_llir(pe.constant, M, B);
  default:
    assert(0);
  }
  return NULL;
}

llvm::Value *to_llir(Constant &con, llvm::Module &M, llvm::IRBuilder<> &B) {
  switch (con.tag) {
  case Constant_int:
    return llvm::ConstantInt::get(
      llvm::Type::getInt32Ty(M.getContext()),
      con.ival
    );
  case Constant_char:
    assert(0);
  default:
    assert(0);
  }
}

}
