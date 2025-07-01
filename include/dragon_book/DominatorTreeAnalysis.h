#pragma once

namespace dragon_book {

void analyzeDominatorTree(llvm::Module &M) {
  llvm::errs() << "analyze dominator tree\n";
  assert(M.getFunctionList().size() == 1);

  llvm::Function &F = *M.begin();

  for (auto &BB : F) {
    llvm::errs() << BB.getName() << " ->\n"; 
    for (auto succ : llvm::successors(&BB)) {
      llvm::errs() << "  " << succ->getName() << "\n";
    }
  }
  assert(0);
}

}
