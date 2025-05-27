#include "llvm/IR/Instructions.h"
#include "llvm/IR/CFG.h"
#include <unordered_map>
#include <unordered_set>
#include <string>
#include <set>

namespace dragon_book {

using BBToStoreSetType = std::unordered_map<llvm::BasicBlock *, std::unordered_set<llvm::StoreInst*>>;
using VarToStoresType = std::unordered_map<std::string, std::vector<llvm::StoreInst*>>;

struct ReachingDefinitionContext {
  llvm::Function *F;
  BBToStoreSetType bbToGen;
  BBToStoreSetType bbToKill;
  std::unordered_map<llvm::StoreInst*, std::string> storeToName;
  BBToStoreSetType bbToIn;
  BBToStoreSetType bbToOut;
};

void iterateUntilFix(ReachingDefinitionContext &RDC) {
  auto &F = *RDC.F;
  auto &bbToIn = RDC.bbToIn;
  auto &bbToOut = RDC.bbToOut;
  auto &bbToGen = RDC.bbToGen;
  auto &bbToKill = RDC.bbToKill;

  llvm::BasicBlock *entry = &F.front();
  assert(entry->getName().str() == "entry");

  llvm::BasicBlock *exit = &F.back();
  assert(exit->getName().str() == "exit");

  // out[ENTRY] = empty
  bbToOut[entry].clear();
  for (auto &BB : F) {
    if (&BB != entry) {
      bbToOut[&BB].clear();
    }
  }

  bool changed = true;
  int iter_idx = 0;
  while (changed) {
    llvm::errs() << "Iteration " << ++iter_idx << "\n";
    changed = false;
    for (auto &BB : F) {
      if (&BB == entry) {
        continue;
      }
      // join
      bbToIn[&BB].clear();
      for (auto pred : llvm::predecessors(&BB)) {
        bbToIn[&BB].insert(bbToOut[pred].begin(), bbToOut[pred].end());
      }

      // transfer
      std::unordered_set<llvm::StoreInst*> newOut;
      newOut = bbToGen[&BB];
      std::unordered_set<llvm::StoreInst*> in_except_kill;
      for (llvm::StoreInst* store : bbToIn[&BB]) {
        if (bbToKill[&BB].count(store) == 0) {
          in_except_kill.insert(store);
        }
      }
      newOut.insert(in_except_kill.begin(), in_except_kill.end());
      if (newOut != bbToOut[&BB]) {
        changed = true;
      }
      bbToOut[&BB] = newOut;
    }
  }
}

void dumpStoreSet(std::unordered_set<llvm::StoreInst *> stores, ReachingDefinitionContext &RDC) {
  std::set<std::string> names; // use set for ordering
  for (auto store : stores) {
    names.insert(RDC.storeToName[store]);
  }
  for (auto name : names) {
    llvm::errs() << " " << name;
  }
}

void dumpGenKillForEachBB(ReachingDefinitionContext &RDC) {
  llvm::Function &F = *RDC.F;
  auto &bbToGen = RDC.bbToGen;
  auto &bbToKill = RDC.bbToKill;
  auto &storeToName = RDC.storeToName;

  for (llvm::BasicBlock &BB : F) {
    std::unordered_set<llvm::StoreInst*> genSet = bbToGen[&BB];
    std::unordered_set<llvm::StoreInst*> killSet = bbToKill[&BB];

    llvm::errs() << BB.getName() << ":\n";
    llvm::errs() << "  gen:";
    dumpStoreSet(genSet, RDC);
    llvm::errs() << "\n";
    llvm::errs() << "  kill:";
    dumpStoreSet(killSet, RDC);
    llvm::errs() << "\n";
  }
}

void dumpCFAResult(ReachingDefinitionContext &RDC) {
  llvm::Function &F = *RDC.F;
  auto &bbToIn = RDC.bbToIn;
  auto &bbToOut = RDC.bbToOut;

  for (auto &BB : F) {
    auto &inSet = bbToIn[&BB];
    auto &outSet = bbToOut[&BB];

    llvm::errs() << BB.getName() << " in:";
    dumpStoreSet(inSet, RDC);
    llvm::errs() << "\n";
    llvm::errs() << BB.getName() << " out:";
    dumpStoreSet(outSet, RDC);
    llvm::errs() << "\n";
  }
}

std::unordered_set<llvm::StoreInst*> getKillSetForSingleStore(llvm::StoreInst *store, VarToStoresType& varToStores) {
  std::string name = store->getOperand(1)->getName().str();
  std::vector<llvm::StoreInst*> stores = varToStores[name];
  std::unordered_set<llvm::StoreInst*> killSet;

  for (llvm::StoreInst *otherStore : stores) {
    if (otherStore != store) {
      killSet.insert(otherStore);
    }
  }

  return killSet;
}

void analyzeReachingDefinition(llvm::Module &M) {
  llvm::errs() << "analyze reaching definition\n";
  ReachingDefinitionContext RDC;
  assert(M.getFunctionList().size() == 1);

  llvm::Function &F = *M.begin();
  RDC.F = &F;

  // give each store a sequential name
  auto &storeToName = RDC.storeToName;

  // group stores for each variable
  VarToStoresType varToStores;

  for (llvm::BasicBlock &BB : F) {
    // llvm::errs() << BB.getName() << "\n";
    for (llvm::Instruction &I : BB) {
      if (llvm::dyn_cast<llvm::StoreInst>(&I)) {
        llvm::StoreInst *store = llvm::cast<llvm::StoreInst>(&I);
        llvm::Value *ptr = store->getOperand(1);

        int idx = storeToName.size() + 1;
        std::string name = std::string("d") + std::to_string(idx);
        storeToName[store] = name;
        varToStores[ptr->getName().str()].push_back(store);
        // llvm::errs() << "  store " << ptr->getName() << "; " << name << "\n";
      }
    }
  }

  // calculate gen/kill set for each basic block except entry and exit
  BBToStoreSetType &bbToGen = RDC.bbToGen;
  BBToStoreSetType &bbToKill = RDC.bbToKill;
  for (llvm::BasicBlock &BB : F) {
    std::unordered_set<llvm::StoreInst*> accKill;
    std::unordered_set<llvm::StoreInst*> accGen;

    for (llvm::Instruction &I : BB) {
      llvm::StoreInst *store = llvm::dyn_cast<llvm::StoreInst>(&I);
      if (!store) {
        continue;
      }

      std::unordered_set<llvm::StoreInst*> killSetForStore = getKillSetForSingleStore(store, varToStores);
      if (accKill.count(store) == 0) {
        accGen.insert(store);
      }

      accKill.insert(killSetForStore.begin(), killSetForStore.end());
    }

    bbToGen[&BB] = accGen;
    bbToKill[&BB] = accKill;
  }

  // uncomment for debugging
  // dumpGenKillForEachBB(RDC);

  iterateUntilFix(RDC);
  dumpCFAResult(RDC);
}

}
