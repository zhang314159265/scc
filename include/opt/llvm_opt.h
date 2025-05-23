#pragma once

#if USE_LLOPT

#include "llvm/IR/PassManager.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Transforms/Scalar/DCE.h"
#include "llvm/Transforms/Scalar/GVN.h"
#include "llvm/Transforms/Utils/Mem2Reg.h"
#include "llvm/Transforms/InstCombine/InstCombine.h"
#include "llvm/Analysis/AliasAnalysis.h"

void llvm_opt(llvm::Module &M) {
  llvm::PassBuilder PB;
  llvm::ModuleAnalysisManager MAM;
  llvm::FunctionAnalysisManager FAM;
  llvm::LoopAnalysisManager LAM;
  llvm::CGSCCAnalysisManager CGAM;

  PB.registerModuleAnalyses(MAM);
  PB.registerFunctionAnalyses(FAM);
  PB.registerLoopAnalyses(LAM);
  PB.registerCGSCCAnalyses(CGAM);
  PB.crossRegisterProxies(LAM, FAM, CGAM, MAM);

  llvm::FunctionPassManager FPM;
  // FPM.addPass(llvm::PromotePass()); // mem-to-reg
  // FPM.addPass(llvm::GVNPass()); // this is CSE pass in LLVM
  // FPM.addPass(llvm::DCEPass());
  FPM.addPass(llvm::InstCombinePass()); // this does constant folding besides other things

  for (llvm::Function &F : M) {
    if (!F.isDeclaration()) {
      FPM.run(F, FAM);
    }
  }
}

#endif
