#pragma once

#include <unordered_map>
#include "ConstantFolding.h"

namespace scc {

class InstCombinePass {
 public:
  void run(llvm::Function &F) {
    for (auto &BB : F) {
      bool changed = true;
      while (changed) {
        std::unordered_map<llvm::Instruction*, llvm::Constant*> inst2const;
        for (auto &I : BB) {
          if (llvm::Constant *C = scc::ConstantFoldInstruction(&I)) {
            inst2const[&I] = C;
          }
        }
        for (auto [I, C] : inst2const) {
          I->replaceAllUsesWith(C);
          I->eraseFromParent();
        }
        changed = (inst2const.size() > 0);
      }
    }
  }
};

}
