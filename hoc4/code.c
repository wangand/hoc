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

/*control functions will go here*/
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

/*boolean functions will go here*/

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

