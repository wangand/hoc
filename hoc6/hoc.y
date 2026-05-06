%{
#include <stdio.h>
#include <signal.h>
#include <setjmp.h>
#include "hoc.h"

int yylex(void);
int yyerror(char* s);
void execerror(char* s, char*t);
void fpecatch(int n);
void defnonly(char *s);
jmp_buf begin;
int indef;
extern double Pow(double x, double y);
extern Inst *code(Inst f);
extern void define(Symbol *sp);

#define code2(c1,c2) code(c1); code(c2)
#define code3(c1,c2,c3) code(c1);code(c2);code(c3);
%}
%union {
 struct Symbol *sym;
 Inst *inst;
 int narg;
}
%token <sym> NUMBER STRING PRINT VAR BLTIN UNDEF WHILE IF ELSE
%token <sym> FUNCTION PROCEDURE RETURN FUNC PROC READ
%token <narg> ARG
%type <inst> expr stmt asgn prlist stmtlist
%type <inst> cond while if begin end
%type <sym> procname
%type <narg> arglist
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
 | list defn '\n'
 | list asgn '\n' { code(STOP); return 1; }
 | list stmt '\n' { code(STOP); return 1; }
 | list expr '\n' { code2(print, STOP); return 1; }
 | list error '\n' { yyerrok; }
;
asgn: VAR '=' expr { $$=$3; code3(varpush,(Inst)$1,assign); }
 | ARG '=' expr {
 defnonly("$"); code2(argassign,(Inst)$1); $$=$3;}
;
stmt: expr { /*code(popstack);*/ }
 | RETURN { defnonly("return"); code(procret); }
 | RETURN expr {
 defnonly("return"); $$=$2; code(funcret);
}
 | PROCEDURE begin '(' arglist ')' {
 $$ = $2; code3(call, (Inst)$1, (Inst)$4);
}
 | PRINT prlist { $$ = $2; }
 | while cond stmt end {
 ($1)[1] = (Inst)$3; /* body of loop */
 ($1)[2] = (Inst)$4 ;/* end, if cond fails */
}
 | if cond stmt end {
 ($1)[1] = (Inst)$3; /* thenpart */
 ($1)[3] = (Inst)$4; /* end, if cond fails */
}
 | if cond stmt end ELSE stmt end{
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
begin: /* nothing */ { $$ = progp; }
;
end: { code(STOP); $$ = progp; }
;
stmtlist: { $$ = progp; }
 | stmtlist '\n'
 | stmtlist stmt
;
expr: NUMBER { $$ = code2(constpush, (Inst)$1); }
 | VAR { $$ = code3(varpush,(Inst)$1, eval); }
 | ARG { defnonly("$"); $$ = code2(arg, (Inst)$1); }
 | asgn
 | FUNCTION begin '(' arglist ')' {
 $$ = $2; code3(call, (Inst)$1, (Inst)$4);
}
 | READ '(' VAR ')' { $$ = code2(varread, (Inst)$3); }
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
prlist: expr { code(prexpr); }
 | STRING { $$ = code2(prstr, (Inst)$1); }
 | prlist ',' expr { code(prexpr); }
 | prlist ',' STRING { code2(prstr, (Inst)$3); }
 ;
defn: FUNC procname { $2->type =FUNCTION; indef=1; }
'(' ')' stmt { code(procret); define($2); indef=0; }
 | PROC procname { $2->type=PROCEDURE; indef=1; }
'(' ')' stmt { code(procret); define($2); indef=0; }
 ;
procname: VAR
 | FUNCTION
 | PROCEDURE
 ;
arglist: /* nothing */ { $$ = 0; }
 | expr { $$ = 1; }
 | arglist ',' expr { $$ = $1 + 1; }
 ;
%%
/* end of grammar */
#include <stdio.h>
#include <ctype.h>
char *progname ; /* for error messages */
int lineno = 1;
char *infile; /* input file name */
FILE *fin; /* input file pointer */
char **gargv; /* global argument list */
int gargc;
extern void init();
extern void initcode();
extern void execute(Inst *p);
extern int moreinput();
void run();

int main(int argc, char *argv[]) {
 int i;
 
 progname = argv[0];
 if(argc==1){
  static char *stdinonly[] = { "-" };
  gargv = stdinonly;
  gargc = 1;
 }
 else{
  gargv = argv+1;
  gargc = argc-1;
 }
 init();
 while(moreinput()){
  run();
 }
 return 0;
}

void run(){
 setjmp(begin);
 signal(SIGFPE, fpecatch);
 for(initcode(); yyparse(); initcode()){
  execute(progbase);
 }
}

void defnonly(char *s){
 if(!indef){
  execerror(s, "used outside definition");
 }
}

int yyerror(char* s){
 fprintf(stderr, "%s\n", s);
 return 0;
}

void fpecatch(int n){
 execerror("floating point exception", (char *) 0);
}
