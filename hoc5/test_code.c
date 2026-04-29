#include <stdio.h>
#include <setjmp.h>
#include "hoc.h"
#include "hoc.tab.h"

jmp_buf begin;
extern void init();
extern void initcode();
extern void execute(Inst *p);
extern Inst *code(Inst f);
extern Inst *pc;
extern void push(Datum d);

void printprog(){
 for(Inst *p=prog; *p != STOP; p++){
  printf("%p\n", *p);
 }
 printf("STOP\n");
}

int main(){
 init();
 int jmptest = 0;
 setjmp(begin);
 if(jmptest==1){
  return 0;
 }
 jmptest = 1;

 printf("testing code\n");

 initcode();

struct Symbol *v = install("", NUMBER, 42);
struct Symbol *s = install("abc", UNDEF, 0.0);
code(constpush);
code((Inst) v);
code(varpush);
code((Inst) s);
code(assign);
code(varpush);
code((Inst) s);
code(eval);
code(print);

/*printprog();*/

 execute(prog);
}
