#pragma once

#if USE_LLOPT

#include "llvm/IR/PassManager.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Transforms/Scalar/DCE.h"
#include "llvm/Transforms/Utils/Mem2Reg.h"

void llvm_opt(llvm::Module &M) {
  llvm::PassBuilder PB;
  llvm::FunctionAnalysisManager FAM;
  PB.registerFunctionAnalyses(FAM);

  llvm::FunctionPassManager FPM;
  FPM.addPass(llvm::PromotePass());
  FPM.addPass(llvm::DCEPass());

  for (llvm::Function &F : M) {
    if (!F.isDeclaration()) {
      FPM.run(F, FAM);
    }
  }
}

#endif
