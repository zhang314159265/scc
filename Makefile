all:
	lex -o tokenizer.cpp tokenizer.l
	yacc parser.y -o parser.cpp
	g++ -I. parser.cpp label.cpp type.cpp symtab.cpp thac.cpp ast/*.cpp -ly -ll -lfmt -std=c++17 -o scc
	cat sample_input.c | ./scc
