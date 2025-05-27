#include <iostream>
#include <cassert>

#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IRReader/IRReader.h"
#include "llvm/Support/SourceMgr.h"

#include "dragon_book/ReachingDefinition.h"

int main(int argc, char **argv) {
  assert(argc == 2);
  const char *irpath = argv[1];

  llvm::LLVMContext C;
  llvm::SMDiagnostic err;

  std::unique_ptr<llvm::Module> pM = llvm::parseIRFile(irpath, err, C);
  if (!pM) {
    llvm::errs() << "Fail to parse file " << irpath << "\n";
    err.print(argv[0], llvm::errs());
    return -1;
  }
  llvm::Module &M = *pM;

  M.dump();

  dragon_book::analyzeReachingDefinition(M);
  return 0;
}
