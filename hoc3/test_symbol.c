#include <stdio.h>
#include <setjmp.h>
#include "hoc.h"
#include "hoc.tab.h"

jmp_buf begin;
extern void execerror(char* s, char* t);

int main(void){
 int jmptest = 0;
 setjmp(begin);
 if(jmptest==1){
  return 0;
 }
 jmptest = 1;
 
 install("PI", VAR, 3.141592);

 printf("testing symbol\n");
}
