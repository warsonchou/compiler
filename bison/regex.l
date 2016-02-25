%{
    #include "symbol.h"
    #include <stdlib.h>
    #include "regex.tab.h"
%}


%%


"+"             return '+';
"*"             return '*';
"("             {increLtp(); return '(';}
")"             { return ')';}
"|"				return '|';
"?"				return '?';
":"				{decreLtp(); return ':';}
"\n"            return '\n';
[^+:*()|?]		{
	int typeIndex = 3;
	if (yytext[0] == '.') {
		typeIndex = 4;
	}
	yylval.nd = createNode(-1, -1, typeIndex, yytext);
	return CH;
}

%%

int yywrap(void) {
    return 1;
}