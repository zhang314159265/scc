#pragma once

namespace dragon_book {

using ValueTypeDom = std::unordered_set<llvm::BasicBlock*>;
using BBToImmDomType = std::unordered_map<llvm::BasicBlock *, llvm::BasicBlock *>;

void dumpCFG(llvm::Function &F) {
  for (auto &BB : F) {
    llvm::errs() << BB.getName() << " ->\n"; 
    for (auto succ : llvm::successors(&BB)) {
      llvm::errs() << "  " << succ->getName() << "\n";
    }
  }
}

std::string valueToStrFuncDom(ValueTypeDom val, void *ignore) {
  std::set<std::string> names; // use set for ordering
  for (llvm::BasicBlock *bb : val) {
    names.insert(bb->getName().str());
  }
  std::stringstream ss;
  for (auto name : names) {
    ss << " " << name;
  }
  return ss.str();
}

void computeImmDom(DFAForward<ValueTypeDom> &dfa, BBToImmDomType &bbToImmDom) {
  for (auto &BB : dfa.F) {
    ValueTypeDom domSet = dfa.bbToOut[&BB];
    // TODO can we use bbToIn directly?
    domSet.erase(&BB);
    if (domSet.empty()) {
      continue;
    }

    auto cmp = [&dfa](llvm::BasicBlock *lhs, llvm::BasicBlock *rhs) -> bool {
      auto &lhsDomSet = dfa.bbToOut[lhs];
      return lhsDomSet.count(rhs) > 0;
    };
    bbToImmDom[&BB] = *std::min_element(
      domSet.begin(),
      domSet.end(),
      cmp);
  }
}

void dumpImmDom(DFAForward<ValueTypeDom> &dfa, BBToImmDomType &bbToImmDom) {
  for (auto &bb : dfa.F) {
    if (bbToImmDom.count(&bb) > 0) {
      llvm::errs() << bb.getName() << " has imm dom " << bbToImmDom[&bb]->getName() << "\n";
    } else {
      llvm::errs() << bb.getName() << " has no dominator\n";
    }
  }
}

void analyzeDominatorTree(llvm::Module &M) {
  llvm::errs() << "analyze dominator tree\n";

  ValueTypeDom topVal;
  for (auto &BB : *M.begin()) {
    topVal.insert(&BB);
  }

  DFAForward<ValueTypeDom> dfa(M, {&M.begin()->front()}, topVal,
    valueToStrFuncDom, nullptr,
    joinFuncIntersect<ValueTypeDom>);

  // uncomment for debugging
  dumpCFG(dfa.F);

  // setup genkill
  for (auto &BB : dfa.F) {
    auto &genSet = dfa.bbToGen[&BB];
    auto &killSet = dfa.bbToKill[&BB];
    genSet.insert(&BB);
  }

  // dfa.dumpGenKill();

  dfa.iterateUntilFix();
  dfa.dumpResult();

  BBToImmDomType bbToImmDom;
  computeImmDom(dfa, bbToImmDom);

  dumpImmDom(dfa, bbToImmDom);
}

}
