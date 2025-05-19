#include <iostream>
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/ValueSymbolTable.h"

using namespace llvm;
using namespace std;

LLVMContext C;
Type *fpTy = Type::getFloatTy(C);
Type *i32Ty = Type::getInt32Ty(C);
Type *i8Ty = Type::getInt8Ty(C);
Type *voidTy = Type::getVoidTy(C);
PointerType *i8PtrTy = PointerType::get(i8Ty, 0);
FunctionType *printfTy = FunctionType::get(i8Ty, {i8PtrTy}, true);
FunctionType *mainTy = FunctionType::get(i32Ty, {}, false);

int main(void) {
  Module M("Manually created IR", C);
  Function::Create(printfTy, Function::ExternalLinkage, "printf", M);

  Function *F = Function::Create(mainTy, Function::ExternalLinkage, "main", M);

  BasicBlock *entry = BasicBlock::Create(C, "entry", F);
  IRBuilder<> B(C);
  B.SetInsertPoint(entry);
  
  {
    llvm::Value *val = llvm::ConstantInt::get(i32Ty, 10);
    llvm::ArrayType *arTy = llvm::ArrayType::get(fpTy, llvm::cast<llvm::ConstantInt>(val)->getZExtValue());
    llvm::Value *ar = B.CreateAlloca(arTy, nullptr, "sum");
    B.CreateGEP(arTy, ar, {llvm::ConstantInt::get(i32Ty, 0), llvm::ConstantInt::get(i32Ty, 5)});
  }

  {
    M.print(llvm::errs(), NULL);
    std::error_code EC;
    const std::string &path = "/tmp/gen.ll";
    llvm::raw_fd_ostream os(path, EC);
    M.print(os, NULL);
    std::cout << "LLIR written to " << path << std::endl;
  
    std::cout << "bye!" << std::endl;
  }

  return 0;
}
