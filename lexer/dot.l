%{
	#include <string.h>
	#include <stdio.h>

		
%}
WHITESPACE [\ \t\n]
ONELINECOMMENT (\/\/.*)|(#.*)

%x COMMENT

KEYWORD (?i:STRICT)|(?i:GRAPH)|(?i:DIGRAPH)|(?i:SUBGRAPH)|(?i:NODE)|(?i:EDGE)
ID [A-Za-z_][A-Za-z0-9_]*
OTHERSKEY (--)|(->)|=
NUMBER -?[0-9]*\.?[0-9]+
SEPARATOR [;,{}\]\[]
STRING \"([^"\\]|\\.)*\"




%%
{KEYWORD}|{STRING}|{ID}|{NUMBER}|{SEPARATOR}|{OTHERSKEY} {
	printf("%s\n", yytext);
}

{WHITESPACE} 				/* eat space*/
{ONELINECOMMENT}			/*eat online comment*/

"/*" 				BEGIN(COMMENT);
<COMMENT>[^*\n]*        /* eat anything thiat's not a '*' */
<COMMENT>"*"+[^*/\n]*   /* eat up '*'s not followed by '/'s */
<COMMENT>\n
<COMMENT>"*"+"/"        BEGIN(INITIAL);

%%

