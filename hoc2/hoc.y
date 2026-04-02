%{
#include <stdio.h>
int yylex(void);
int yyerror(char* s);
double mem[26];
%}
%union {
 double val;
 int index;
}
%token <val> NUMBER
%token <index> VAR
%type <val> expr
%left '+' '-' 
%left '*' '/'
%left UNARYMINUS
%%
list: /* nothing*/
 | list '\n'
 | list expr '\n' { printf("\t%.8g\n", $2); }
;
expr: NUMBER { $$ = $1; }
 | VAR { $$ = mem[$1]; }
 | VAR '=' expr { $$ = mem[$1] = $3; }
 | expr '+' expr {  $$ = $1 + $3; }
 | expr '-' expr {  $$ = $1 - $3; }
 | expr '*' expr {  $$ = $1 * $3; }
 | expr '/' expr {  $$ = $1 / $3; }
 | '(' expr ')' { $$ = $2; }
 | '-' expr %prec UNARYMINUS { $$ = -$2; }
%%
/* end of grammar */
#include <stdio.h>
#include <ctype.h>
char *progname ; /* for error messages */
int lineno = 1;

int main(int argc, char *argv[]) {
 progname = argv[0];
 yyparse();
}

int yyerror(char* s){
 fprintf(stderr, "%s", s);
 return 0;
}

