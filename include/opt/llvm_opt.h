#pragma once

#if USE_LLOPT

#include "llvm/IR/PassManager.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Transforms/Scalar/DCE.h"
#include "llvm/Transforms/Scalar/GVN.h"
#include "llvm/Transforms/Utils/Mem2Reg.h"
#include "llvm/Transforms/InstCombine/InstCombine.h"

void llvm_opt(llvm::Module &M) {
  llvm::PassBuilder PB;
  llvm::FunctionAnalysisManager FAM;
  PB.registerFunctionAnalyses(FAM);

  llvm::FunctionPassManager FPM;
  FPM.addPass(llvm::PromotePass()); // mem-to-reg
  // FPM.addPass(llvm::GVNPass()); // TODO: this is CSE pass in LLVM. it requires more analysis passes.
  // FPM.addPass(llvm::DCEPass());
  // FPM.addPass(llvm::InstCombinePass()); // TODO: this does constant folding, but it requires more analysis passes

  for (llvm::Function &F : M) {
    if (!F.isDeclaration()) {
      FPM.run(F, FAM);
    }
  }
}

#endif
