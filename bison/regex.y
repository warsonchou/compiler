/* Infix notation calculator. */

%{
    #include "symbol.h"
    #include <stdio.h>
    #include <math.h>
    #include <memory.h>
    #include <stdlib.h>
    Node nodes[max];
    int isVisited[max];
    int matchNum = 0, lfp = 0, rtp = 0;  
    char nodeType[12][10] = {"Paren", "Cat", "Alt", "Lit", "Dot", "Star", "Plus", "Quest", "NgStar", "NgPlus", "NgQuest", ""};
    Node * createNode(int lf, int rt, int nodeTypeIndex, char *val);
    void preOrder(int root);
    void increLtp() {lfp++;}
    void decreLtp() {lfp--;}
    void yyerror (char const *);
%}

%union {
    Node *nd;
}

%token <nd> CH

%left '|'
%right '('
%left CH
%left UMINUS
%left '*' '+'
%right '?'
%left ':'
%left ')'

%type  <nd> exp

%% 
/* Grammar rules and actions follow.  */

input : /* empty */
      | input line
;

line  : '\n'
      | exp '\n'                {
                                  preOrder($1->index);
                                  printf("\n");
                                  matchNum = 0;
                                  lfp = rtp = 0;
                                  memset(isVisited, 0, sizeof(int) * max);        }
;

exp   : CH                      { $$ = $1;                                }
      | exp '*'                 { $$ = createNode($1 -> index, -1, 5, "*");      }
      | exp '?'                 { $$ = createNode($1 -> index, -1, 7, "?");      }
      | exp '+'                 { $$ = createNode($1 -> index, -1, 6, "+");      }
      | exp '*' '?'             { $$ = createNode($1 -> index, -1, 8, "*?");     }
      | exp '+' '?'             { $$ = createNode($1 -> index, -1, 9, "+?");     }
      | exp '?' '?'             { $$ = createNode($1 -> index, -1, 10, "??");    }
      | '(' '?' ":" exp ')'     { $$ = $4;                                }
      | '(' exp ')'             {
                                  int which = lfp - rtp + matchNum; //第几个paren
                                  rtp++;
                                  if (lfp == rtp) {
                                    matchNum = lfp;
                                  }
                                  char str[10];
                                  sprintf(str, "%d", which);
                                  int i;
                                  for (i = 0; str[i] != '\0'; i++) {
                                    nodeType[11][i] = str[i];
                                  }
                                  nodeType[11][i] = '\0';
                                  Node *templf = createNode(-1, -1, 11, str);
                                  $$ = createNode(templf -> index, $2 -> index, 0, str);      }
      | '(' '?' ':' exp ')'     { $$ = $4;                                }
      | exp exp %prec UMINUS    { $$ = createNode($1 -> index, $2 -> index, 1, "");         }
      | exp '|' exp             { $$ = createNode($1 -> index, $3 -> index, 2, "|");        }
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

struct Node * createNode(int lf, int rt, int nodeTypeIndex, char *appendChar) {
  Node *temp;
  int i, j, k;
  for (k = 0; k < max; k++) {
    if (!isVisited[k]) {
      isVisited[k] = 1;
      temp = nodes + k;
      temp->lf = lf;
      temp->rt = rt;
      temp->index = temp - nodes;
      temp->type = nodeTypeIndex;
      for (i = 0; nodeType[nodeTypeIndex][i] != '\0'; i++) {
        temp->val[i] = nodeType[nodeTypeIndex][i];
      }

      // if the node type Lit
      if (nodeTypeIndex == 3) {
        temp->val[i++] = '(';
        for (j = 0; appendChar[j] != '\0'; j++) {
            temp->val[i++] = appendChar[j];
        }
        temp->val[i++] = ')';
        temp->val[i] = '\0';
      } else {
        temp->val[i] = '\0';
      }
      break;
    }
  }
  if (temp == nodes + max) {
    yyerror("Array overflow!");
  }
  return temp;
}

void preOrder(int root) {
  if (root != -1) {

    printf("%s", nodes[root].val);
    // not leaf
    if (!(nodes[root].rt == -1 && nodes[root].lf == -1)) {
      printf("(");
    }
    preOrder(nodes[root].lf);

    // cat, paren, alt
    if (nodes[root].type == 0 || nodes[root].type == 1 || nodes[root].type == 2) {
      printf(", ");
    }

    preOrder(nodes[root].rt);
    // not leaf
    if (!(nodes[root].rt == -1 && nodes[root].lf == -1)) {
      printf(")");
    }
  }
}


