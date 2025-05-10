LLVM_ROOT := ../llvm-project
CLANG := $(LLVM_ROOT)/build/bin/clang
CFLAGS := -I/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include -Wno-nullability-completeness
LDFLAGS := -L /Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/lib

all:
	# lldb -- 
	$(CLANG) $(CFLAGS) $(LDFLAGS) sum.c
	./a.out
