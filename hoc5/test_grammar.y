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
extern Inst *code(Inst f);

#define code2(c1,c2) code(c1); code(c2)
#define code3(c1,c2,c3) code(c1);code(c2);code(c3);
%}
%union {
 struct Symbol *sym;
 Inst *inst;
}
%token <sym> NUMBER PRINT VAR BLTIN UNDEF WHILE IF ELSE
%type <inst> stmt asgn expr if
%right '='
%left OR
%left AND
%left GT GE LT LE EQ NE
%left '+' '-' 
%left '*' '/'
%left UNARYMINUS NOT
%right '^'
%%
list: /* nothing*/
 | list '\n'
 | list asgn '\n' { code(STOP); return 1; }
 | list stmt '\n' { code(STOP); return 1; }
 | list expr '\n' { code2(print, STOP); return 1; }
 | list error '\n' { yyerrok; }
;
asgn: VAR '=' expr { $$=$3; code3(varpush,(Inst)$1,assign); }
;
stmt: PRINT expr {code(prexpr); $$=$2;}
 | if cond
;
cond: '(' expr ')' {printf("cond\n");}
;
if: IF { printf("IF\n"); }
;
expr: NUMBER { $$ = code2(constpush, (Inst)$1); }
 | VAR { $$ = code3(varpush, (Inst)$1, eval); }
 | '(' expr ')' { $$ = $2; }
 | expr '+' expr { code(add); }
 | expr '-' expr { code(sub); }
 | expr '*' expr { code(mul); }
 | expr '/' expr { code(hocdiv); }
 | expr '^' expr { code(power); }
 | '-' expr %prec UNARYMINUS { $$ = $2; code(negate); }
 ;
%%
/* end of grammar */
#include <stdio.h>
#include <ctype.h>
char *progname ; /* for error messages */
int lineno = 1;
extern void init();
extern void initcode();
extern void execute(Inst *p);

int main(int argc, char *argv[]) {
 progname = argv[0];
 init();
 setjmp(begin);
 signal(SIGFPE, fpecatch);
 for(initcode(); yyparse(); initcode()){
  execute(prog);
 }
 return 0;
}

int yyerror(char* s){
 fprintf(stderr, "%s\n", s);
 return 0;
}

void fpecatch(int n){
 execerror("floating point exception", (char *) 0);
}
