#include <iostream>
#include <cassert>

#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IRReader/IRReader.h"
#include "llvm/Support/SourceMgr.h"

#include "dragon_book/ReachingDefinition.h"
#include "dragon_book/DominatorTreeAnalysis.h"

int main(int argc, char **argv) {
  assert(argc == 3);
  std::string cmd(argv[1]);
  const char *irpath = argv[2];

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

  if (cmd == "reaching_definition") {
    dragon_book::analyzeReachingDefinition(M);
  } else if (cmd == "dominator_tree") {
    dragon_book::analyzeDominatorTree(M);
  } else {
    llvm::errs() << "Unrecognized command " << cmd << "\n";
    return -1;
  }
  return 0;
}
