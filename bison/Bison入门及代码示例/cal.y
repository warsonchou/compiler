/* Infix notation calculator. */

%{
    #include <stdio.h>
    #include <math.h>
    void yyerror (char const *);
%}

%token NUM
%left  '-' '+'
%left  '*' '/'
%right '^'       /* exponentiation */

%% 
/* Grammar rules and actions follow.  */

input : /* empty */
      | input line
;

line  : '\n'
      | exp '\n'            { printf("\t=> %d\n", $1); }
;

exp   : NUM                 { $$ = $1;          }
      | '(' exp ')'         { $$ = $2;          }
      | exp '+' exp         { $$ = $1 + $3;     }
      | exp '-' exp         { $$ = $1 - $3;     }
      | exp '*' exp         { $$ = $1 * $3;     }
      | exp '/' exp         { $$ = $1 / $3;     }
      | exp '^' exp         { $$ = pow($1, $3); }
;

%%

int main() 
{
    return yyparse();
}

/* Called by yyparse on error.  */
void yyerror (char const *s)
{
    fprintf (stderr, "%s\n", s);
}


