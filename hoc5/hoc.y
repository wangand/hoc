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
%type <inst> stmt asgn expr stmtlist cond while if end
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
stmt: expr { /*code(popstack);*/ }
 | PRINT expr { code(prexpr); $$ = $2; }
 | while cond stmt end {
 ($1)[1] = (Inst)$3; /* body of loop */
 ($1)[2] = (Inst)$4 ;/* end, if cond fails */
}
 | if cond stmt end {
 ($1)[1] = (Inst)$3; /* thenpart */
 ($1)[3] = (Inst)$4; /* end, if cond fails */
}
 | if cond stmt end ELSE stmt end{ printf("?else\n");
 ($1)[1] = (Inst)$3; /* thenpart */
 ($1)[2] = (Inst)$6; /* elsepart */
 ($1)[3] = (Inst)$7; /* end, if cond fails */
}
 | '{' stmtlist '}' { $$ = $2; }
;
cond: '(' expr ')' { code(STOP); $$ = $2; }
;
while: WHILE { $$ = code3(whilecode, STOP, STOP); }
;
if: IF { $$=code(ifcode); code3(STOP, STOP, STOP); }
;
end: { code(STOP); $$ = progp; }
;
stmtlist: { $$ = progp; }
 | stmtlist '\n'
 | stmtlist stmt
;
expr: NUMBER { $$ = code2(constpush, (Inst)$1); }
 | VAR { $$ = code3(varpush,(Inst)$1, eval); }
 | asgn
 | BLTIN '(' expr ')' { $$ = code2(bltin, (Inst)$1->u.ptr); }
 | '(' expr ')' { $$ = $2; }
 | expr '+' expr { code(add); }
 | expr '-' expr { code(sub); }
 | expr '*' expr { code(mul); }
 | expr '/' expr { code(hocdiv); }
 | expr '^' expr { code(power); }
 | '-' expr %prec UNARYMINUS { $$ = $2; code(negate); }
 | expr GT expr { code(gt); }
 | expr GE expr { code(ge); }
 | expr LT expr { code(lt); }
 | expr LE expr { code(le); }
 | expr EQ expr { code(eq); }
 | expr NE expr { code(ne); }
 | expr AND expr { code(and); }
 | expr OR expr { code(or); }
 | NOT expr { $$ = $2; code(not); }
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
