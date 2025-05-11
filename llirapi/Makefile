LLVM_ROOT := ../llvm-project
LLVM_CONFIG := $(LLVM_ROOT)/build/bin/llvm-config

LLVMAPI_FLAGS := `$(LLVM_CONFIG) --cxxflags --ldflags --system-libs --libs core`
CFLAGS :=
LDFLAGS := -L /opt/homebrew/Cellar/zstd/1.5.7/lib

all:
	clang++ $(CFLAGS) $(LDFLAGS) $(LLVMAPI_FLAGS) showmsg.cpp
	rm /tmp/gen.ll
	./a.out
	clang -Wno-override-module /tmp/gen.ll -o /tmp/generated
	/tmp/generated
