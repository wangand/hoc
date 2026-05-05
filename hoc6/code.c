#include <stdio.h>
#include "hoc.h"
#include "hoc.tab.h"
extern double Pow(double, double);
extern int moreinput();

#define NSTACK 256
static Datum stack[NSTACK];
static Datum *stackp;

#define NPROG 2000
Inst prog[NPROG];
Inst *progp; /* next free spot for code generation */
Inst *pc; /* program counter */
Inst *progbase = prog; /* start of current subprogram */
int returning; /* 1 if return stmt seen */

typedef struct Frame{ /* proc/func call stack frame */
 Symbol *sp; /* symbol table entry */
 Inst *retpc; /* symbol table entry */
 Datum *argn; /* n-th argument on stack */
 int nargs; /* number of arguments */
} Frame;

#define NFRAME 100
Frame frame[NFRAME];
Frame *fp; /* frame pointer */

void execute(Inst *p);

void initcode(){
 progp = progbase;
 stackp = stack;
 fp = frame;
 returning = 0;
}

void push(Datum d){
 if(stackp >= &stack[NSTACK]){
  execerror("stack overflow", (char *)0);
 }
 *stackp++ = d;
}

Datum pop(){
 if(stackp <= stack){
  execerror("stack underflow", (char *)0);
 }
 return *--stackp;
}

int constpush(){
 Datum d;
 d.val = ((Symbol *)*pc++)->u.val;
 push(d);
 return 1;
}

int varpush(){
 Datum d;
 d.sym = (Symbol *)(*pc++);
 push(d);
 return 1;
}

int whilecode(){
 Datum d;
 Inst *savepc = pc; /* loop body */
 
 execute(savepc+2); /* condition */
 d = pop();
 while(d.val){
  execute(*((Inst **)(savepc))); /* body */
  if(returning){
   break;
  }
  execute(savepc+2);
  d = pop();
 }
 if(!returning){
  pc = *((Inst **)(savepc+1)); /* next statement */
 }
 return 1;
}

int ifcode(){
 Datum d;
 Inst *savepc = pc; /* then part */
 
 execute(savepc+3); /* condition */
 d = pop();
 if(d.val){
  execute(*((Inst **)(savepc)));
 }
 else if (*((Inst **)(savepc+1))){ /* else part? */
  execute(*((Inst **)(savepc+1))); 
 }
 if(!returning){
  pc = *((Inst **)(savepc+2)); /* next stmt */
 }
 return 1;
}

void define(Symbol *sp){
 sp->u.defn = (Inst)progbase; /* start of code */
 progbase = progp; /* next code starts here */
}

int call(){ /* call a function */
 Symbol *sp = (Symbol *)pc[0]; /* symbol table entry */
 if(fp++ >= &frame[NFRAME-1]){
  execerror(sp->name, "call nested too deeply");
 }
 fp->sp = sp;
 fp->nargs = (int)pc[1];
 fp->retpc = pc + 2;
 fp->argn = stackp - 1; /* last argument */
 execute(sp->u.defn);
 returning = 0;
 return 1;
}

int ret(){ /* common return from func or proc */
 int i;
 for(i=0; i<fp->nargs; i++){
  pop();
 }
 pc = (Inst *)fp->retpc;
 --fp;
 returning = 1;
 return 1;
}

int funcret(){
 Datum d;
 if(fp->sp->type == PROCEDURE){
  execerror(fp->sp->name, "(proc) returns value");
 }
 d = pop();
 ret();
 push(d);
 return 1;
}

int procret(){
 if(fp->sp->type == FUNCTION){
  execerror(fp->sp->name, "(func) returns no value");
 }
 ret();
 return 1;
}

double *getarg(){ /* return from a procedure */
 int nargs = (int) *pc++;
 if(nargs > fp->nargs){
  execerror(fp->sp->name, "not enough arguments");
 }
 return &fp->argn[nargs - fp->nargs].val;
}

int arg(){ /* push argument onto stack */
 Datum d;
 d.val = *getarg();
 push(d);
 return 1;
}

int argassign(){ /* store top of stack in argument */
 Datum d;
 d = pop();
 push(d); /* leave value on stack */
 *getarg() = d.val;
 return 1; 
}

int bltin(){
 Datum d;
 d = pop();
 d.val = ((double (*)(double))(*pc++))(d.val);
 push(d);
 return 1; 
}

int eval(){
 Datum d;
 d = pop();
 if(d.sym->type == UNDEF){
  execerror("undefined variable", d.sym->name);
 }
 d.val = d.sym->u.val;
 push(d);
 return 1;
}

int add(){
 Datum d1, d2;
 d2 = pop();
 d1 = pop();
 d1.val += d2.val;
 push(d1);
 return 1;
}

int sub(){
 Datum d1, d2;
 d2 = pop();
 d1 = pop();
 d1.val -= d2.val;
 push(d1);
 return 1;
}

int mul(){
 Datum d1, d2;
 d2 = pop();
 d1 = pop();
 d1.val *= d2.val;
 push(d1);
 return 1;
}

int hocdiv(){
 Datum d1, d2;
 d2 = pop();
 if(d2.val==0.0){
  execerror("division by zero", (char *)0);
 }
 d1 = pop();
 d1.val /= d2.val;
 push(d1);
 return 1;
}

int negate(){
 Datum d;
 d = pop();
 d.val = -d.val;
 push(d);
 return 1;
}

int gt(){
 Datum d1, d2;
 d2 = pop();
 d1 = pop();
 d1.val = (double)(d1.val > d2.val);
 push(d1);
 return 1;
}

int lt(){
 Datum d1, d2;
 d2 = pop();
 d1 = pop();
 d1.val = (double)(d1.val < d2.val);
 push(d1);
 return 1; 
}

int ge(){
 Datum d1, d2;
 d2 = pop();
 d1 = pop();
 d1.val = (double)(d1.val >= d2.val);
 push(d1);
 return 1; 
}

int le(){
 Datum d1, d2;
 d2 = pop();
 d1 = pop();
 d1.val = (double)(d1.val <= d2.val);
 push(d1);
 return 1;
}

int eq(){
 Datum d1, d2;
 d2 = pop();
 d1 = pop();
 d1.val = (double)(d1.val == d2.val);
 push(d1);
 return 1;
}

int ne(){
 Datum d1, d2;
 d2 = pop();
 d1 = pop();
 d1.val = (double)(d1.val != d2.val);
 push(d1);
 return 1;
}

int and(){
 Datum d1, d2;
 d2 = pop();
 d1 = pop();
 d1.val = (double)(d1.val != 0.0 && d2.val != 0.0);
 push(d1);
 return 1;
}

int or(){
 Datum d1, d2;
 d2 = pop();
 d1 = pop();
 d1.val = (double)(d1.val !=0.0 ||  d2.val != 0.0);
 push(d1);
 return 1;
}

int not(){
 Datum d;
 d = pop();
 d.val = (double)(d.val == 0.0);
 push(d);
 return 1;
}

int power(){
 Datum d1, d2;
 d2 = pop();
 d1 = pop();
 d1.val = Pow(d1.val, d2.val);
 push(d1);
 return 1;
}

int assign(){
 Datum d1, d2;
 d1 = pop();
 d2 = pop();
 if(d1.sym->type != VAR && d1.sym->type != UNDEF){
  execerror("assignment to non-variable", d1.sym->name);
 }
 d1.sym->u.val = d2.val;
 d1.sym->type = VAR;
 return 1;
}

int print(){
 Datum d;
 d = pop();
 printf("\t%.8g\n", d.val);
 return 1;
}

int prexpr(){ /* print numeric value */
 Datum d;
 d = pop();
 printf("%.8g\n", d.val);
 return 1;
}

int prstr(){ /* print string value */
 printf("%s", (char *) *pc++);
 return 1;
}

int varread(){ /* read into variable */
 Datum d;
 Symbol *var = (Symbol *) *pc++;
Again:
 switch(fscanf(stdin, "%lf", &var->u.val)){
  case EOF:
   if(moreinput()){
    goto Again;
   }
   d.val = var->u.val = 0.0;
   break;
  case 0:
   execerror("non-number read into", var->name);
   break;
  default:
   d.val = 1.0;
   break;
 }
 var->type = VAR;
 push(d);
 return 1;
}

Inst *code(Inst f){
 Inst *oprogp = progp;
 if(progp >= &prog[NPROG]){
  execerror("program too big", (char *)0);
 }
 *progp++ = f;
 return oprogp;
}

void execute(Inst *p){
 for(pc=p; *pc != STOP; ){
  (*(*pc++))();
  /* same effect: */
  /* (*pc++)(); */
 }
}

int popstack(){
 pop();
 return 1;
}
