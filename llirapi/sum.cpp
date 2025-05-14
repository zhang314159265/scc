#include <iostream>
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/ValueSymbolTable.h"

using namespace llvm;
using namespace std;

LLVMContext C;
Type *i32Ty = Type::getInt32Ty(C);
Type *i8Ty = Type::getInt8Ty(C);
// Type *voidTy = Type::getVoidTy(C);
PointerType *i8PtrTy = PointerType::get(i8Ty, 0);
FunctionType *printfTy = FunctionType::get(i8Ty, {i8PtrTy}, true);
FunctionType *mainTy = FunctionType::get(i32Ty, {}, false);

int main(void) {
  Module M("Manually created IR", C);
  Function::Create(printfTy, Function::ExternalLinkage, "printf", M);

  Function *F = Function::Create(mainTy, Function::ExternalLinkage, "main", M);

  BasicBlock *entry = BasicBlock::Create(C, "entry", F);
  BasicBlock *condCheck = BasicBlock::Create(C, "condCheck", F);
  BasicBlock *body = BasicBlock::Create(C, "body", F);
  BasicBlock *exit = BasicBlock::Create(C, "exit", F);

  IRBuilder<> B(C);

  { // entry
    B.SetInsertPoint(entry);
    B.CreateAlloca(i32Ty, nullptr, "sum");
    B.CreateStore(
      ConstantInt::get(i32Ty, 0),
      F->getValueSymbolTable()->lookup("sum")
    );
    B.CreateAlloca(i32Ty, nullptr, "i");
    B.CreateStore(
      ConstantInt::get(i32Ty, 0),
      F->getValueSymbolTable()->lookup("i")
    );
    B.CreateBr(condCheck);
  }

  { // condCheck
    llvm::IRBuilder<> B(C); // Creating a separate IRBuilder is also fine
    B.SetInsertPoint(condCheck);
    Value *i = B.CreateLoad(i32Ty, F->getValueSymbolTable()->lookup("i"));
    Value *cmpres = B.CreateICmpSLE(i,
      ConstantInt::get(i32Ty, 100));
    B.CreateCondBr(cmpres, body, exit);
  }

  { // body
    B.SetInsertPoint(body);
    Value *sumadd = B.CreateAdd(
      B.CreateLoad(i32Ty, F->getValueSymbolTable()->lookup("sum")),
      B.CreateLoad(i32Ty, F->getValueSymbolTable()->lookup("i")));
    B.CreateStore(
      sumadd,
      F->getValueSymbolTable()->lookup("sum")
    );
    Value *iadd = B.CreateAdd(
      B.CreateLoad(i32Ty, F->getValueSymbolTable()->lookup("i")),
      ConstantInt::get(i32Ty, 1));
    B.CreateStore(
      iadd,
      F->getValueSymbolTable()->lookup("i"));
    B.CreateBr(condCheck);
  }

  { // exit
    B.SetInsertPoint(exit);
    Function *printf = M.getFunction("printf");
    Value *str = B.CreateGlobalString("Sum is %d\n");
    Value *sum = B.CreateLoad(i32Ty, F->getValueSymbolTable()->lookup("sum"));

    B.CreateCall(printf->getFunctionType(), printf,
      {str, sum});
    B.CreateRet(ConstantInt::get(i32Ty, 0));
  }

  M.print(llvm::errs(), NULL);
  std::error_code EC;
  const std::string &path = "/tmp/gen.ll";
  llvm::raw_fd_ostream os(path, EC);
  M.print(os, NULL);
  std::cout << "LLIR written to " << path << std::endl;

  std::cout << "bye!" << std::endl;

  return 0;
}
