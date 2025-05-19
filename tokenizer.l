%{
#include "token.h"
#include <iostream>

#if STANDALONE_TOKENIZER
#include "SemanticValue.h"

scc::SemanticValue yylval;
#endif

%}

LETTER_ [a-zA-Z_]
DIGIT [0-9]

%%
[ \t\r\n]+ { /* ignore spaces */ }
[/][/].* { /* ignore comment */ }
[/][*](.|\n)*[*][/] { /* ignore comment */ }

int { return TK_INT; }
float { return TK_FLOAT; }
double { return TK_DOUBLE; }
const { return TK_CONST; }
volatile { return TK_VOLATILE; }
unsigned { return TK_UNSIGNED; }
char { return TK_CHAR; }
void { return TK_VOID; }
return { return TK_RETURN; }
for { return TK_FOR; }
if { return TK_IF; }
else { return TK_ELSE; }

[.][.][.] { return TK_ELLIPSIS; }
[<][=] { return TK_LE; }
[+][+] { return TK_INC; }
[+][=] { return TK_PLUS_EQ; }

["]([^"]*)["] { 
  // don't support escaped double-quote yet
  yylval = scc::SemanticValue(yytext + 1, strlen(yytext) - 2); return TK_STR;
}

['].['] {
  yylval = scc::SemanticValue((int) yytext[1]); return TK_CHAR_CONST;
}

{DIGIT}+ { int ival = atoi(yytext); yylval = scc::SemanticValue(ival); return TK_INT_CONST; }
{DIGIT}+[.]{DIGIT}+ { double dval = atof(yytext); yylval = scc::SemanticValue(dval); return TK_FLOAT_CONST; }

{LETTER_}({LETTER_}|{DIGIT})* { yylval = scc::SemanticValue(yytext); return TK_IDENTIFIER; }

. { return yytext[0]; }

%%

#if STANDALONE_TOKENIZER
int main() {
  int tok;

  // yylex returns 0 when done
  while ((tok = yylex())) {
    printf("Got token %d '%s'\n", tok, tok_to_str(tok));
    switch (tok) {
    case TK_IDENTIFIER:
    case TK_STR:
    case TK_INT_CONST:
    {
      std::cout << yylval << std::endl;
      break;
    }
    }
  }
  return 0;
}
#endif
