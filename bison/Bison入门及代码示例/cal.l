%{
    #include <stdlib.h>
    #include "cal.tab.h"
%}

%%

[0-9]+          { yylval = atoi(yytext); return NUM; } 
"+"             return '+';
"-"             return '-';
"*"             return '*';
"/"             return '/';
"^"             return '^';
"("             return '(';
")"             return ')';
"\n"            return '\n';
[ \t]+        ;       /* ignore whitespace */

.               yyerror("Unknown character");

%%

int yywrap(void) {
    return 1;
}