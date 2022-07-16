LETTER_ [a-zA-Z_]
DIGIT [0-9]

%%
[ \t\r\n]+ { /* ignore spaces */ }
[/][/].* { /* ignore comment */ }
{DIGIT}+ { int ival = atoi(yytext); yylval = SemanticValue(new IntConst(ival)); return INTEGER_CONSTANT; }

int { return TK_INT; }
double { return TK_DOUBLE; }

for { return FOR; }
while { return WHILE; }
[-][-] { return DEC; }
[+][+] { return INC; }
-- { return DEC; }
{LETTER_}+({LETTER_}|{DIGIT})* { yylval = SemanticValue(yytext); return IDENTIFIER; }

. { return yytext[0]; }
%%
