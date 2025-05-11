#include <iostream>
#include <string>
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Module.h"

llvm::LLVMContext C;
llvm::Type *i32Ty = llvm::Type::getInt32Ty(C);
llvm::Type *i8Ty = llvm::Type::getInt8Ty(C);
llvm::Type *i8PtrTy = llvm::PointerType::get(i8Ty, 0);

int main(void) {
  llvm::IRBuilder<> B(C);
  llvm::Module M("showmsg created by llvm IR API", C);

  llvm::FunctionType *mainFuncType = llvm::FunctionType::get(i32Ty, {}, false);
  llvm::Function *F = llvm::Function::Create(
    mainFuncType,
    llvm::Function::ExternalLinkage,
    "main",
    M);

  llvm::BasicBlock *BB = llvm::BasicBlock::Create(
    C,
    "entry",
    F);

  B.SetInsertPoint(BB);

  llvm::GlobalVariable *msg = B.CreateGlobalString("LLVM is cool!\n", "msg");

  llvm::FunctionCallee printf = M.getOrInsertFunction(
    "printf",
    llvm::FunctionType::get(
      i32Ty,
      i8PtrTy,
      true
    )
  );

  B.CreateCall(printf, msg);
  B.CreateRet(
    llvm::ConstantInt::get(i32Ty, 0)
  );

  M.print(llvm::errs(), NULL);
  std::error_code EC;
  const std::string &path = "/tmp/gen.ll";
  llvm::raw_fd_ostream os(path, EC);
  M.print(os, NULL);
  std::cout << "LLIR written to " << path << std::endl;

  std::cout << "bye!" << std::endl;
  return 0;
}
