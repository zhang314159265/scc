#pragma once

#include "llvm/IR/Instructions.h"
#include "llvm/IR/CFG.h"
#include <unordered_map>
#include <unordered_set>
#include <string>
#include <sstream>
#include <set>
#include "dragon_book/DFA.h"

namespace dragon_book {

using ValueType = std::unordered_set<llvm::StoreInst*>;
using VarToStoresType = std::unordered_map<std::string, std::vector<llvm::StoreInst*>>;
using StoreToNameType = std::unordered_map<llvm::StoreInst*, std::string>;

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

void setupGenKill(DFAForward<ValueType> &dfa, StoreToNameType &storeToName) {
  // group stores for each variable
  VarToStoresType varToStores;

  for (llvm::BasicBlock &BB : dfa.F) {
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
  auto &bbToGen = dfa.bbToGen;
  auto &bbToKill = dfa.bbToKill;
  for (llvm::BasicBlock &BB : dfa.F) {
    ValueType accKill;
    ValueType accGen;

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
}

std::string valueToStrRD(ValueType val, void *arg) {
  assert(arg);
  StoreToNameType &storeToName = *(StoreToNameType *) arg;
  std::stringstream ss;

  std::set<std::string> names; // use set for ordering
  for (auto elem : val) {
    names.insert(storeToName[elem]);
  }
  for (auto name : names) {
    ss << " " << name;
  }

  return ss.str();
}

void analyzeReachingDefinition(llvm::Module &M) {
  llvm::errs() << "analyze reaching definition\n";
  DFAForward<ValueType> dfa(M, {}, {}, valueToStrRD, nullptr);

  // give each store a sequential name
  StoreToNameType storeToName;
  setupGenKill(dfa, storeToName);
  dfa.valueToStrArg = (void *) &storeToName;

  // uncomment for debugging
  // dfa.dumpGenKill();

  dfa.iterateUntilFix();
  dfa.dumpResult();
}

}
