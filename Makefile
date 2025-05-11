tokenizer:
	lex -o tokenizer.cpp tokenizer.l
	clang++ -Iinclude -ll tokenizer.cpp
	./a.out < inputs/showmsg.c

all:
	make -C proto

try_llirapi:
	make -C llirapi
