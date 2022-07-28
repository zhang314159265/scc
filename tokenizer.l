LETTER_ [a-zA-Z_]
DIGIT [0-9]

%%
[ \t\r\n]+ { /* ignore spaces */ }
[/][/].* { /* ignore comment */ }
[/][*](.|\n)*[*][/] { std::cout << "Ignore comment '" << yytext << "'" << std::endl; /* ignore comment */ }
{DIGIT}+ { int ival = atoi(yytext); yylval = SemanticValue(new IntConst(ival)); return INTEGER_CONSTANT; }

{DIGIT}+[.]{DIGIT}+ { double dval = atof(yytext); yylval = SemanticValue(new DoubleConst(dval)); return FLOATING_CONSTANT; }

int { return TK_INT; }
double { return TK_DOUBLE; }

[<][=] { return TK_LE; }
[>][=] { return TK_GE; }
[=][=] { return TK_EQ; }
[!][=] { return TK_NE; }

if { return IF; }
else { return ELSE; }
for { return FOR; }
while { return WHILE; }
[-][-] { return DEC; }
[+][+] { return INC; }
-- { return DEC; }
{LETTER_}+({LETTER_}|{DIGIT})* { yylval = SemanticValue(yytext); return IDENTIFIER; }

. { return yytext[0]; }
%%
