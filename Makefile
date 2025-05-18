LLVM_ROOT := llvm-project
LLVM_CONFIG := $(LLVM_ROOT)/build/bin/llvm-config
LLVMAPI_FLAGS := `$(LLVM_CONFIG) --cxxflags --ldflags --system-libs --libs core`
CFLAGS :=
LDFLAGS := -L /opt/homebrew/Cellar/zstd/1.5.7/lib

scc:
	lex -o tokenizer.cpp tokenizer.l
	bison parser.y -o parser.cpp
	# clang++ $(CFLAGS) $(LDFLAGS) $(LLVMAPI_FLAGS) -Iinclude -ll -DSTANDALONE_TOKENIZER tokenizer.cpp all.cpp
	# ./a.out < inputs/sum.c
	clang++ -g $(CFLAGS) $(LDFLAGS) $(LLVMAPI_FLAGS) -Iinclude -ll -ly parser.cpp all.cpp -o scc
	# process launch -i inputs/showmsg.c
	rm -f /tmp/gen.ll
	# ./scc < inputs/sum.c
	./scc < inputs/misc.c
	# ./scc < inputs/showmsg.c
	# false
	clang++ /tmp/gen.ll
	./a.out

.PHONY: test scc

test:
	./test.sh
  
genllir:
	clang -S -emit-llvm inputs/sum.c -o /tmp/sum.ll

all:
	make -C proto

try_llirapi:
	make -C llirapi
