# Passes
- DCEPass : include/llvm/Transforms/Scalar/DCE.h
- Mem2Reg
- InstCombine: why this pass alone can optimize `inputs/saved_misc/constant_folding_example.c` to the optimal? It works without running Mem2Reg pass.

# Later
- check `make_early_inc_range` in include/llvm/ADT/STLExtras.h
- Find an example of DFA in llvm (seems to be hard to find)

# TODO
- DominatorTreeAnalysis: it's NOT based on DFA but it's used in a lot of important passes
  - .dominates is an important method
