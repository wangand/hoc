#include <stdio.h>
#include <setjmp.h>
#include "hoc.h"
#include "hoc.tab.h"

jmp_buf begin;
extern void init();
extern void initcode();
extern void execute(Inst *p);


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
 execute(prog);
}
