LETTER_ [a-zA-Z_]
DIGIT [0-9]

%%
[ \t\r\n]+ { /* ignore spaces */ }
[/][/].* { /* ignore comment */ }
{DIGIT}+ { int ival = atoi(yytext); yylval = SemanticValue(new IntConst(ival)); return INTEGER_CONSTANT; }

for { return FOR; }
while { return WHILE; }
{LETTER_}+({LETTER_}|{DIGIT})* { yylval = SemanticValue(yytext); return IDENTIFIER; }

. { return yytext[0]; }
%%
