# Passes
- DCEPass : include/llvm/Transforms/Scalar/DCE.h
- Mem2Reg

# Later
- check `make_early_inc_range` in include/llvm/ADT/STLExtras.h

# TODO
- DominatorTreeAnalysis: it's NOT based on DFA
- InstCombine: why this pass alone can optimize misc.c to the optimal? It works without running Mem2Reg pass. <++++
  - try to do constant folding following InstCombine: <=== HERE
- Find an example of DFA in llvm (seems to be hard to find) <===
- try to implement a DFA myself following the dragon book <====
