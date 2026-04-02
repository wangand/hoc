%{
#include <stdio.h>
int yylex(void);
int yyerror(char* s);
%}
%union {double val;}
%token <val>NUMBER
%type <val> expr
%left '+' '-' 
%left '*' '/'
%%
list: /* nothing*/
 | list '\n'
 | list expr '\n' { printf("\t%.8g\n", $2); }
;
expr: NUMBER { $$ = $1; }
 | expr '+' expr {  $$ = $1 + $3; }
 | expr '-' expr {  $$ = $1 - $3; }
 | expr '*' expr {  $$ = $1 * $3; }
 | expr '/' expr {  $$ = $1 / $3; }
 | '(' expr ')' { $$ = $2; }
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

