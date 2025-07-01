#pragma once

#include <unordered_map>
#include <functional>

namespace dragon_book {

template <typename ValueType>
ValueType txFuncSet(ValueType input, ValueType gen, ValueType kill) {
  ValueType out = gen;
  ValueType in_except_kill;
  for (auto elem : input) {
    if (kill.count(elem) == 0) {
      in_except_kill.insert(elem);
    }
  }
  out.insert(in_except_kill.begin(), in_except_kill.end());
  return out;
}

template <typename ValueType>
ValueType joinFuncUnion(ValueType lhs, ValueType rhs) {
  ValueType out = lhs;
  out.insert(rhs.begin(), rhs.end());
  return out;
}

template <typename ValueType>
ValueType joinFuncIntersect(ValueType lhs, ValueType rhs) {
  ValueType out;
  for (auto &lhsVal : lhs) {
    if (rhs.count(lhsVal) > 0) {
      out.insert(lhsVal);
    }
  }
  return out;
}

// forward
template <typename ValueType>
class DFAForward {
 public:
  using BBToValueType = std::unordered_map<llvm::BasicBlock*, ValueType>;
  using JoinFuncType = std::function<ValueType(ValueType, ValueType)>;
  using TransferFuncType = std::function<ValueType(ValueType, ValueType, ValueType)>;
  using ValueToStrFunc = std::function<std::string(ValueType, void*)>;
  DFAForward(
    llvm::Module &M_, ValueType entryValue_, ValueType topValue_,
    ValueToStrFunc valueToStrFunc_,
    void *valueToStrArg_,
    JoinFuncType joinFunc_ = joinFuncUnion<ValueType>,
    TransferFuncType txFunc_ = txFuncSet<ValueType>
  )
    : M(M_), F(*M.begin()), 
      entryValue(entryValue_), topValue(topValue_),
      valueToStrFunc(valueToStrFunc_),
      valueToStrArg(valueToStrArg_),
      joinFunc(joinFunc_), txFunc(txFunc_) {
    assert(M.getFunctionList().size() == 1);
  }

  ValueType joinMulti(std::vector<ValueType>& inputs) {
    assert(inputs.size() > 0);
    ValueType out = inputs[0];
    for (int i = 1; i < inputs.size(); ++i) {
      out = joinFunc(out, inputs[i]);
    }
    return out;
  }

  void iterateUntilFix() {
    llvm::BasicBlock *entry = &F.front();
    bbToOut[entry] = entryValue;
    for (auto &BB : F) {
      if (&BB != entry) {
        bbToOut[&BB] = topValue;
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
        std::vector<ValueType> joinInputs;
        for (auto pred : llvm::predecessors(&BB)) {
          joinInputs.push_back(bbToOut[pred]);
        }
        bbToIn[&BB] = joinMulti(joinInputs);


        // transfer
        ValueType newOut = txFunc(
          bbToIn[&BB], bbToGen[&BB], bbToKill[&BB]);
        if (newOut != bbToOut[&BB]) {
          changed = true;
        }
        bbToOut[&BB] = newOut;
      }
    }
  }

  void dumpValue(ValueType &val) {
    if (valueToStrFunc) {
      llvm::errs() << valueToStrFunc(val, valueToStrArg);
    } else {
      llvm::errs() << "(missing valueToStrFunc)";
    }
  }

  void dumpGenKill() {
    for (llvm::BasicBlock &BB : F) {
      ValueType genSet = bbToGen[&BB];
      ValueType killSet = bbToKill[&BB];
  
      llvm::errs() << BB.getName() << ":\n";
      llvm::errs() << "  gen:";
      dumpValue(genSet);
      llvm::errs() << "\n";
      llvm::errs() << "  kill:";
      dumpValue(killSet);
      llvm::errs() << "\n";
    }
  }

  void dumpResult() {
    for (auto &BB : F) {
      auto &inSet = bbToIn[&BB];
      auto &outSet = bbToOut[&BB];
  
      llvm::errs() << BB.getName() << " in:";
      dumpValue(inSet);
      llvm::errs() << "\n";
      llvm::errs() << BB.getName() << " out:";
      dumpValue(outSet);
      llvm::errs() << "\n";
    }
  }
 public:
  llvm::Module &M;
  llvm::Function &F;

  BBToValueType bbToGen, bbToKill, bbToIn, bbToOut;

  ValueType entryValue;
  ValueType topValue;
  JoinFuncType joinFunc;
  TransferFuncType txFunc;

  ValueToStrFunc valueToStrFunc;
  void *valueToStrArg; // an optional arg for ValueToStrFunc
};

}
