#pragma once

namespace scc {

class Mem2RegPass {
 public:
  void run(llvm::Module &M) {
    this->M = &M;
    for (auto &F : M) {
      if (!F.isDeclaration()) {
        run(F);
      }
    }
  }

  void run(llvm::Function &F) {
    for (llvm::BasicBlock &BB : F) {
      std::vector<llvm::AllocaInst*> allocas;
      for (llvm::Instruction &I : BB) {
        if (llvm::AllocaInst *alloca = llvm::dyn_cast<llvm::AllocaInst>(&I)) {
          allocas.push_back(alloca);
        }
      }
      for (llvm::AllocaInst *alloca : allocas) {
        if (!handleOneAlloca(alloca)) {
          alloca->dump();
          assert(0 && "can not handle this alloca so far");
        }
      }
    }
  }

  static int getInstPos(llvm::Instruction *I) {
    llvm::BasicBlock *BB = I->getParent();
    int idx = 0;
    for (llvm::Instruction &child : *BB) {
      if (&child == I) {
        return idx;
      }
      ++idx;
    }
    assert(0);
  }

  bool handleOneAlloca(llvm::AllocaInst *alloca) {
    llvm::BasicBlock *BB = alloca->getParent();

    int minLoadPos = BB->size();
    int storePos = -1;
    llvm::StoreInst *onlyStore = nullptr;

    for (llvm::User *U : alloca->users()) {
      if (llvm::dyn_cast<llvm::LoadInst>(U)) {
        minLoadPos = std::min(minLoadPos, getInstPos(llvm::cast<llvm::Instruction>(U)));
      } else if (llvm::dyn_cast<llvm::StoreInst>(U)) {
        if (storePos >= 0) {
          assert(0 && "multi store");
        }
        storePos = getInstPos(llvm::cast<llvm::Instruction>(U));
        onlyStore = llvm::cast<llvm::StoreInst>(U);
      } else {
        assert(0);
      }
    }
    assert(storePos >= 0);
    assert(storePos < minLoadPos);

    llvm::Value *replValue = onlyStore->getOperand(0);

    // replace the users of each LoadInst
    std::vector<llvm::LoadInst *> loadToRepl;
    for (llvm::User *U : alloca->users()) {
      llvm::LoadInst *loadInst = llvm::dyn_cast<llvm::LoadInst>(U);
      if (!loadInst) {
        continue;
      }
      loadToRepl.push_back(loadInst);
    }
    for (auto loadInst : loadToRepl) {
      loadInst->replaceAllUsesWith(replValue);
      loadInst->eraseFromParent();
    }

    // Now the store/alloca is also useless
    onlyStore->eraseFromParent();
    alloca->eraseFromParent();

    return true;
  }
 private:
  llvm::Module *M = nullptr;
};

}
