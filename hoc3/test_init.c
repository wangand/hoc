#include <stdio.h>
#include <setjmp.h>
#include "hoc.h"
#include "hoc.tab.h"

jmp_buf begin;
extern void init();

int main(){
 init();
 int jmptest = 0;
 setjmp(begin);
 if(jmptest==1){
  return 0;
 }
 jmptest = 1;

 Symbol *sp;
 sp = lookup("PI");
 if(sp==0) printf("PI not found\n");
 else printf("%lf\n",sp->u.val);

 sp = lookup("sqrt");
 if(sp==0) printf("sqrt not found\n");
 else{
  double retval;
  retval = ((double (*)(double))(sp->u.ptr))(9.0);
  printf("%lf\n", retval);
 }

 printf("testing init\n");
}
