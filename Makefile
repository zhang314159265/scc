tokenizer:
	lex -o tokenizer.cpp tokenizer.l
	bison parser.y -o parser.cpp
	# clang++ -Iinclude -ll -DSTANDALONE_TOKENIZER tokenizer.cpp 
	# ./a.out < inputs/showmsg.c
	clang++ -Iinclude -ll -ly parser.cpp
	./a.out < inputs/showmsg.c

all:
	make -C proto

try_llirapi:
	make -C llirapi
