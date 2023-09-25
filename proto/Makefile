# INP := inputs/init_identity_matrix.c
# INP := inputs/simp_if_else.c
INP := inputs/matrix_multiply.c
CFLAGS := -I/opt/homebrew/Cellar/fmt/10.1.1/include/
LDFLAGS := -L/opt/homebrew/Cellar/fmt/10.1.1/lib

all:
	lex -o tokenizer.cpp tokenizer.l
	yacc parser.y -o parser.cpp
	g++ -I. parser.cpp label.cpp type.cpp symtab.cpp thac.cpp ast/*.cpp -ly -ll -lfmt -std=c++17 -o scc $(CFLAGS) $(LDFLAGS)
	cat $(INP) | ./scc
