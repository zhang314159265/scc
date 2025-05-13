LLVM_ROOT := llvm-project
LLVM_CONFIG := $(LLVM_ROOT)/build/bin/llvm-config
LLVMAPI_FLAGS := `$(LLVM_CONFIG) --cxxflags --ldflags --system-libs --libs core`
CFLAGS :=
LDFLAGS := -L /opt/homebrew/Cellar/zstd/1.5.7/lib

tokenizer:
	lex -o tokenizer.cpp tokenizer.l
	bison parser.y -o parser.cpp
	# clang++ $(CFLAGS) $(LDFLAGS) $(LLVMAPI_FLAGS) -Iinclude -ll -DSTANDALONE_TOKENIZER tokenizer.cpp 
	# ./a.out < inputs/misc.c
	clang++ -g $(CFLAGS) $(LDFLAGS) $(LLVMAPI_FLAGS) -Iinclude -ll -ly parser.cpp all.cpp
	# process launch -i inputs/showmsg.c
	rm /tmp/gen.ll
	./a.out < inputs/showmsg.c
	# ./a.out < inputs/misc.c
	clang++ /tmp/gen.ll
	./a.out

all:
	make -C proto

try_llirapi:
	make -C llirapi
