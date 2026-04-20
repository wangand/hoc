#include <stdio.h>
#include "hoc.h"
#include "hoc.tab.h"
extern double Pow(double, double);

#define NSTACK 256
static Datum stack[NSTACK];
static Datum *stackp;

#define NPROG 2000
Inst prog[NPROG];
Inst *progp; /* next free spot for code generation */
Inst *pc; /* program counter */

void initcode(){
 stackp = stack;
 progp = prog;
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
  execute(savepc+2);
  d = pop();
 }
 pc = *((Inst **)(savepc+1)); /* next statement */
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
 
 }
 pc = *((Inst **)(savepc+2)); /* next stmt */
 return 1;
}

int prexpr(){
 Datum d;
 d = pop();
 printf("%.8g\n", d.val);
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

int popstack(){
 pop();
 return 1;
}

