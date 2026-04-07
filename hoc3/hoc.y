%{
#include <stdio.h>
#include <signal.h>
#include <setjmp.h>
#include "hoc.h"

int yylex(void);
int yyerror(char* s);
void execerror(char* s, char*t);
void fpecatch(int n);
jmp_buf begin;
extern double Pow(double x, double y);
%}
%union {
 double val;
 struct Symbol *sym;
}
%token <val> NUMBER
%token <sym> VAR BLTIN UNDEF
%type <val> expr asgn
%right '='
%left '+' '-' 
%left '*' '/'
%left UNARYMINUS
%right '^'
%%
list: /* nothing*/
 | list '\n'
 | list asgn '\n'
 | list expr '\n' { printf("\t%.8g\n", $2); }
 | list error '\n' { yyerrok; }
;
asgn: VAR '=' expr { $$=$1->u.val=$3; $1->type=VAR; }
;
expr: NUMBER
 | VAR {
 if($1->type == UNDEF){
  execerror("undefined variable", $1->name);
 }
 $$ = $1->u.val;
}
 | asgn
 | BLTIN '(' expr ')' { $$ = ((double (*)(double))(*($1->u.ptr)))($3); }
 | expr '+' expr {  $$ = $1 + $3; }
 | expr '-' expr {  $$ = $1 - $3; }
 | expr '*' expr {  $$ = $1 * $3; }
 | expr '/' expr {
    if($3==0.0){ execerror("division by zero",""); }
    $$ = $1 / $3; 
   }
 | expr '^' expr { $$ = Pow($1, $3); }
 | '(' expr ')' { $$ = $2; }
 | '-' expr %prec UNARYMINUS { $$ = -$2; }
 ;
%%
/* end of grammar */
#include <stdio.h>
#include <ctype.h>
char *progname ; /* for error messages */
int lineno = 1;

int main(int argc, char *argv[]) {
 progname = argv[0];
 setjmp(begin);
 signal(SIGFPE, fpecatch);
 yyparse();
}

int yyerror(char* s){
 fprintf(stderr, "%s", s);
 return 0;
}

void fpecatch(int n){
 execerror("floating point exception", (char *) 0);
}
