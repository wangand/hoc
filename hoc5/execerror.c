#include <stdio.h>
#include <setjmp.h>
#include "hoc.tab.h"

extern jmp_buf begin;

void execerror(char* s, char* t){
 printf("%s %s\n", s, t);
 longjmp(begin,0);
}

