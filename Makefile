LLVM_ROOT := llvm-project
LLVM_CONFIG := $(LLVM_ROOT)/build/bin/llvm-config

ifeq ($(USE_LLOPT), 1)
LLVMAPI_FLAGS := `$(LLVM_CONFIG) --cxxflags --ldflags --system-libs --libs core passes`
else
LLVMAPI_FLAGS := `$(LLVM_CONFIG) --cxxflags --ldflags --system-libs --libs core irreader support`
USE_LLOPT := 0
endif

CFLAGS :=
LDFLAGS := -L /opt/homebrew/Cellar/zstd/1.5.7/lib

first: opt

scc:
	lex -o tokenizer.cpp tokenizer.l
	bison parser.y -o parser.cpp
	# clang++ $(CFLAGS) $(LDFLAGS) $(LLVMAPI_FLAGS) -Iinclude -ll -DSTANDALONE_TOKENIZER tokenizer.cpp all.cpp -o scc
	# ./scc < inputs/misc.c
	# false
	clang++ -g $(CFLAGS) $(LDFLAGS) $(LLVMAPI_FLAGS) -Iinclude -ll -ly parser.cpp all.cpp -o scc -DUSE_LLOPT=$(USE_LLOPT)
	# process launch -i inputs/misc.c
	rm -f /tmp/gen.ll
	./scc < inputs/misc.c
	# ./scc < inputs/saved_misc/constant_folding_example.c
	# false
	clang++ /tmp/gen.ll
	./a.out

opt:
	clang++ $(CFLAGS) $(LDFLAGS) $(LLVMAPI_FLAGS) -Iinclude opt.cpp -o opt
	./opt dominator_tree llinputs/dominator_tree.ll
	# ./opt reaching_definition llinputs/reaching_definition.ll

.PHONY: test scc opt

test:
	./test.sh

test_opt:
	make opt
	./test_opt.sh
  
genllir:
	clang -S -emit-llvm inputs/sum.c -o /tmp/sum.ll

all:
	make -C proto

try_llirapi:
	make -C llirapi
