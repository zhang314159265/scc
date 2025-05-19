#pragma once
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/ValueSymbolTable.h"
#include "semantic_value/TranslationUnit.h"
#include <iostream>

namespace scc {

struct LoweredDeclaration {
  std::string name;
  llvm::Type *type;
  llvm::Value *initializer = nullptr;
};

struct LowerContext {
  llvm::LLVMContext* C;
  llvm::Module* M;

  // optional fields. can be nullptr.
  llvm::Function *F = nullptr; // used to lookup local variable
  llvm::IRBuilder<> *B = nullptr;

  void dumpModule() {
    M->dump();
  }
};

}

#include "to_llir_helper.h"

namespace scc {

void to_llir(TranslationUnit& tu, LowerContext &LC);
void to_llir(ExternalDeclaration& ed, LowerContext &LC);
LoweredDeclaration to_llir(Declaration& d, LowerContext &LC);
LoweredDeclaration to_llir(DirectDeclarator &dd, llvm::Type *type, LowerContext &LC);
LoweredDeclaration to_llir(Declarator &de, llvm::Type* type, LowerContext &LC);
LoweredDeclaration to_llir(InitDeclarator &id, llvm::Type* type, LowerContext &LC);
llvm::Type *to_llir(DeclarationSpecifiers &ds, LowerContext &LC);
llvm::Type *to_llir(TypeSpecifier &ts, LowerContext &LC);
llvm::Type *to_llir(Pointer &pointer, llvm::Type *type);
void to_llir(Statement &st, LowerContext &LC);
void to_llir(CompoundStatement &cs, LowerContext &LC);
void to_llir(ExpressionStatement &es, LowerContext &LC);
void to_llir(JumpStatement &js, LowerContext &LC);
void to_llir(IterationStatement &is, LowerContext &LC);
void to_llir(SelectiveStatement &ss, LowerContext &LC);

llvm::Value *to_llir(Expression &e, LowerContext &LC);
llvm::Value *to_llir(AssignmentExpression &ae, LowerContext &LC);
llvm::Value *to_llir(PostfixExpression &ce, LowerContext &LC);
llvm::Value *to_llir(PrimaryExpression &ce, LowerContext &LC);
llvm::Value *to_llir(Constant &con, LowerContext &LC);
llvm::Value *to_llir(UnaryExpression &ue, LowerContext &LC);
llvm::Value *to_llir(RelationalExpression &re, LowerContext &LC);
llvm::Value *to_llir(AdditiveExpression &ae, LowerContext &LC);
llvm::Value *to_llir(MultiplicativeExpression &me, LowerContext &LC);

void to_llir(TranslationUnit& tu, LowerContext &LC) {
  for (auto &external_declaration : tu.items) {
    to_llir(external_declaration, LC);
  }
}

void to_llir(ExternalDeclaration& ed, LowerContext &LC) {
  llvm::Module &M = *LC.M;
  switch (ed.tag) {
  case ExternalDeclaration_FunctionDefinition: {
    auto &def = ed.function_definition;
    llvm::Type *type = to_llir(def.declaration_specifiers, LC);
    LoweredDeclaration ld = to_llir(def.declarator, type, LC);
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

    assert(LC.B == nullptr);
    assert(LC.F == nullptr);
    LC.B = &B;
    LC.F = F;
    to_llir(def.compound_statement, LC);
    LC.B = nullptr;
    LC.F = nullptr;
    break;
  }
  case ExternalDeclaration_Declaration: {
    auto loweredDeclaration = to_llir(ed.declaration, LC);
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

LoweredDeclaration to_llir(Declaration& d, LowerContext &LC) {
  llvm::Module &M = *LC.M;
  
  llvm::Type *type = to_llir(d.declaration_specifiers, LC);

  if (d.init_declarator_list.items.size() == 0) {
    return {std::string(), type};
  }

  // return the result of the last InitDecorator
  LoweredDeclaration lowered_decl{std::string(), nullptr};
  for (auto &init_declarator : d.init_declarator_list.items) {
    lowered_decl = to_llir(init_declarator, type, LC);
    if (LC.B) { // local variable
      llvm::IRBuilder<> &B = *LC.B;
      llvm::AllocaInst * alloca = B.CreateAlloca(lowered_decl.type, nullptr, lowered_decl.name);
      // handle initializer
      if (lowered_decl.initializer) {
        handleStore(alloca, lowered_decl.initializer, LC);
      }
    }
  }

  return lowered_decl;
}

llvm::Type *to_llir(DeclarationSpecifiers &ds, LowerContext &LC) {
  llvm::Module &M = *LC.M;
  assert(ds.speclist.size() == 1);

  llvm::Type *type = to_llir(ds.speclist[0], LC);
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

llvm::Type *to_llir(TypeSpecifier &ts, LowerContext& LC) {
  llvm::LLVMContext &C = *LC.C;
  switch (ts) {
  case INT:
    return llvm::Type::getInt32Ty(C);
  case FLOAT:
    return llvm::Type::getFloatTy(C);
  case DOUBLE:
    return llvm::Type::getDoubleTy(C);
  case CHAR:
    return llvm::Type::getInt8Ty(C);
  case VOID:
    return llvm::Type::getVoidTy(C);
  default:
    assert(0);
  }
  return NULL;
}

LoweredDeclaration to_llir(InitDeclarator &id, llvm::Type* type, LowerContext &LC) {
  llvm::Value *initializer = nullptr;
  Initializer &initCST = id.initializer;
  if (initCST.tag == Initializer_Expr) {
    initializer = to_llir(initCST.assignment_expression, LC);
  }
  LoweredDeclaration ld = to_llir(id.declarator, type, LC);
  ld.initializer = initializer;
  return ld;
}

LoweredDeclaration to_llir(Declarator &de, llvm::Type* type, LowerContext &LC) {
  type = to_llir(de.pointer, type);
  return to_llir(de.direct_declarator, type, LC);
}

llvm::Type *to_llir(Pointer &pointer, llvm::Type *type) {
  for (TypeQualifierList quallist : pointer.quallists) {
    // ignore qualifiers
    type = llvm::PointerType::get(type, 0);
  }
  return type;
}

LoweredDeclaration to_llir(DirectDeclarator &dd, llvm::Type *type, LowerContext& LC) {
  const std::string &name = dd.identifier;
  std::vector<DeclarationList> decl_list_list = dd.decl_list_list;
  auto &constant_expressions = dd.constant_expressions;

  LoweredDeclaration lowered_decl = {name, type};

  if (decl_list_list.size() > 0) {
    assert(decl_list_list.size() == 1);
    // function declaration
    DeclarationList &decl_list = decl_list_list[0];
    std::vector<llvm::Type*> argTypes;
    for (Declaration &decl : decl_list.items) {
      LoweredDeclaration ld = to_llir(decl, LC);
      // ld.name does not matter
      argTypes.push_back(ld.type);
    }

    if (argTypes.size() == 1 && argTypes[0]->isVoidTy()) {
      argTypes.clear(); 
    }

    // create function type
    llvm::FunctionType *funcType = llvm::FunctionType::get(
      lowered_decl.type,
      argTypes,
      decl_list.hasEllipsis
    );
    lowered_decl.type = funcType;
  }

  if (constant_expressions.size() > 0) {
    // process from right to left
    for (int i = constant_expressions.size() - 1; i >= 0; --i) {
      llvm::Value *cvalptr = to_llir(*constant_expressions[0], LC);
      lowered_decl.type = llvm::ArrayType::get(
        lowered_decl.type,
        llvm::cast<llvm::ConstantInt>(cvalptr)->getZExtValue()
      );
    }
  }

  return lowered_decl;
}

void to_llir(CompoundStatement &cs, LowerContext &LC) {
  for (auto &declaration : cs.declaration_list.items) {
    LoweredDeclaration lowered_decl = to_llir(declaration, LC);
  }
  for (auto &statement : cs.statement_list.items) {
    to_llir(statement, LC);
  }
}

void to_llir(Statement &st, LowerContext &LC) {
  switch (st.tag) {
  case Statement_ExpressionStatement:
    to_llir(st.expression_statement, LC);
    break;
  case Statement_JumpStatement:
    to_llir(st.jump_statement, LC);
    break;
  case Statement_IterationStatement:
    to_llir(*st.iteration_statement, LC);
    break;
  case Statement_CompoundStatement:
    to_llir(*st.compound_statement, LC);
    break;
  case Statement_SelectiveStatement:
    to_llir(*st.selective_statement, LC);
    break;
  default:
    std::cout << st.tag << std::endl;
    assert(0);
  }
}

void to_llir(JumpStatement &js, LowerContext& LC) {
  llvm::Module &M = *LC.M;
  llvm::IRBuilder<> &B = *LC.B;

  switch (js.tag) {
  case JumpStatement_RETURN: {
    llvm::Value *val = to_llir(js.expression, LC);  
    B.CreateRet(val);
    break;
  }
  default:
    assert(0);
  }
}

void to_llir(ExpressionStatement &es, LowerContext &LC) {
  llvm::Module &M = *LC.M;
  llvm::IRBuilder<> &B = *LC.B;

  to_llir(es.expression, LC);
}

llvm::Value *to_llir(Expression &e, LowerContext &LC) {
  // return llvm::Value* for the last assignment expression
  llvm::Value *value = NULL;
  // size can be 0 for opt_expression use in for statement
  // assert(e.items.size() > 0);
  for (auto &assignmentExpression : e.items) {
    value = to_llir(assignmentExpression, LC);
  }
  return value;
}

llvm::Value *to_llir(AssignmentExpression &ae, LowerContext &LC) {
  llvm::Value *rhs = to_llir(ae.conditional_expression, LC);
  for (auto pair : ae.lhsList) {
    llvm::Value *lhs = to_llir(pair.first, LC);
    AssignmentOperator op = pair.second;
    handleAssignment(lhs, rhs, op, LC);
  }
  return rhs;
}

llvm::Value *to_llir(PostfixExpression &pe, LowerContext &LC) {
  llvm::LLVMContext &C = *LC.C;
  llvm::IRBuilder<> &B = *LC.B;
  llvm::Value *result = to_llir(pe.primary_expression, LC);

  for (auto postfix : pe.postfixList) {
    if (postfix.tag == PostfixExpression_CALL) {
      std::vector<llvm::Value *> args;
      for (auto& assignmentExpr : postfix.argument_expression_list_ptr->items) {
        args.push_back(derefIfNeeded(to_llir(assignmentExpr, LC), LC));
      }
    
      llvm::Type *primary_type = result->getType();
      auto ptr_type = llvm::cast<llvm::PointerType>(primary_type);
      auto function = llvm::cast<llvm::Function>(result);
      auto functionType = function->getFunctionType();
      llvm::FunctionCallee callee(functionType, result);
    
      if (!handleCastIfPrintf(function, args, LC)) {
        handleCastForCall(function, args, LC);
      }
      result = B.CreateCall(callee, args);
    } else if (postfix.tag == PostfixExpression_INDEX) {
      llvm::Value *index = derefIfNeeded(to_llir(*postfix.expression, LC), LC);
  
      // result[index]
      result =  B.CreateGEP(
        getRealType(result),
        result,
        {llvm::ConstantInt::get(llvm::Type::getInt32Ty(C), 0), index}
      );
    } else {
      assert(0 && "unsupported postfix");
    }
  }
  return result;
}

llvm::Value *to_llir(PrimaryExpression &pe, LowerContext &LC) {
  // TODO: how to handle primary expression for global variable?
  llvm::Module &M = *LC.M;
  llvm::IRBuilder<> &B = *LC.B;

  switch (pe.tag) {
  case PrimaryExpression_IDENTIFIER: {
    return lookupIdentifier(pe.str, LC);
  }
  case PrimaryExpression_STR: {
    return B.CreateGlobalString(handleStringEscape(pe.str));
  }
  case PrimaryExpression_constant:
    return to_llir(pe.constant, LC);
  default:
    assert(0);
  }
  return NULL;
}

llvm::Value *to_llir(Constant &con, LowerContext &LC) {
  llvm::Module &M = *LC.M;
  llvm::LLVMContext &C = *LC.C;

  switch (con.tag) {
  case Constant_int:
    return llvm::ConstantInt::get(
      llvm::Type::getInt32Ty(C),
      con.ival
    );
  case Constant_double:
    return llvm::ConstantFP::get(
      llvm::Type::getDoubleTy(C),
      con.dval);
  case Constant_char:
    assert(0);
  default:
    assert(0);
  }
}

llvm::Value *to_llir(UnaryExpression &ue, LowerContext &LC) {
  
  switch (ue.tag) {
  case UnaryExpression_PostfixExpression:
    return to_llir(ue.postfix_expression, LC);
  case UnaryExpression_PREINC: {
    llvm::IRBuilder<> &B = *LC.B;
    llvm::Value *var = to_llir(*ue.unary_expression, LC);
    llvm::AllocaInst *inst = llvm::cast<llvm::AllocaInst>(var);
    llvm::Type *type = inst->getAllocatedType();
    llvm::Value *val = B.CreateLoad(type, var);
    // TODO handle floating point
    llvm::Value *one = llvm::ConstantInt::get(type, 1);
    llvm::Value *result = B.CreateAdd(val, one);
    B.CreateStore(result, var);
    return result;
  }
  default:
    assert(0);
  }
}

llvm::Value *to_llir(RelationalExpression &re, LowerContext &LC) {
  llvm::Value *firstitem = to_llir(re.firstitem, LC);
  if (re.pairlist.size() == 0) {
    return firstitem;
  }

  if (re.pairlist.size() == 1) {
    auto pair = re.pairlist[0];
    llvm::Value *rhs = to_llir(pair.second, LC);
    return handleRelational(firstitem, rhs, pair.first, LC);
  }

  std::cout << "Fail to handle relational expression" << std::endl;
  std::cout << re << std::endl;
  assert(0);
}

void to_llir(IterationStatement &is, LowerContext &LC) {
  assert(is.tag == IterationStatement_FOR); // TODO only support for loop so far
  assert(LC.B);
  llvm::LLVMContext &C = *LC.C;
  llvm::Function *F = LC.F;
  llvm::IRBuilder<> &B = *LC.B;

  // handle expr1, don't care about the value
  to_llir(is.expr1, LC);

  // Create BasicBlocks
  llvm::BasicBlock *condCheck = llvm::BasicBlock::Create(C, "condCheck", F);
  llvm::BasicBlock *body = llvm::BasicBlock::Create(C, "body", F);
  llvm::BasicBlock *exit = llvm::BasicBlock::Create(C, "exit", F);

  // jump to condCheck
  B.CreateBr(condCheck);

  { // handle condCheck
    B.SetInsertPoint(condCheck);
    llvm::Value *val = to_llir(is.expr2, LC);
    assert(val != nullptr); // TODO handle empty conditon check
    B.CreateCondBr(val, body, exit);
  }

  { // handle body
    B.SetInsertPoint(body);
    to_llir(is.stmt, LC);
    to_llir(is.expr3, LC);
    B.CreateBr(condCheck);
  }

  // setup LC.B properly for later
  B.SetInsertPoint(exit);
}

llvm::Value *to_llir(AdditiveExpression &ae, LowerContext &LC) {
  llvm::Value *result = to_llir(ae.firstitem, LC);
  for (auto& pair : ae.pairlist) {
    auto op = pair.first;
    llvm::Value *rhs = to_llir(pair.second, LC);
    result = handleAdditive(result, rhs, op, LC);
  }
  return result;
}

void to_llir(SelectiveStatement &ss, LowerContext &LC) {
  // TODO only support ifelse so far
  assert(ss.tag == SelectiveStatement_IFELSE);
  llvm::IRBuilder<> &B = *LC.B;
  llvm::LLVMContext &C = *LC.C;
  llvm::Function *F = LC.F;

  // Create BasicBlocks
  llvm::BasicBlock *trueBlock = llvm::BasicBlock::Create(C, "trueBlock", F);
  llvm::BasicBlock *falseBlock = llvm::BasicBlock::Create(C, "falseBlock", F);
  llvm::BasicBlock *nextBlock = llvm::BasicBlock::Create(C, "nextBlock", F);

  llvm::Value *condval = to_llir(ss.expr, LC);
  B.CreateCondBr(condval, trueBlock, falseBlock);

  { // handle true block
    B.SetInsertPoint(trueBlock);
    to_llir(ss.stmt1, LC);
    B.CreateBr(nextBlock);
  }

  {
    // handle false block
    B.SetInsertPoint(falseBlock);
    to_llir(ss.stmt2, LC);
    B.CreateBr(nextBlock);
  }

  // properly setup LC.B for later
  B.SetInsertPoint(nextBlock);
}

llvm::Value *to_llir(MultiplicativeExpression &me, LowerContext &LC) {
  llvm::Value *result = to_llir(me.firstitem, LC);
  for (auto& pair : me.pairlist) {
    auto op = pair.first;
    llvm::Value *rhs = to_llir(pair.second, LC);
    result = handleMultiplicative(result, rhs, op, LC);
  }
  return result;
}

}
