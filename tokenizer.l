%%
[ \t\r\n]+ { /* ignore spaces */ }
[/][/].* { /* ignore comment */ }
[0-9]+ { int ival = atoi(yytext); yylval = SemanticValue(new IntConst(ival)); return INTEGER_CONSTANT; }
. { return yytext[0]; }
%%
