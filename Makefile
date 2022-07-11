all:
	lex -o tokenizer.cpp tokenizer.l
	yacc parser.y -o parser.cpp
	g++ -I. parser.cpp label.cpp ast/*.cpp -ly -ll -lfmt -std=c++14 -o scc
	cat sample_input.c | ./scc
