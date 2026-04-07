#include <stdio.h>
#include <ctype.h>
#include "hoc.tab.h"
#include <setjmp.h>

jmp_buf begin;
int yylex(void);
int lineno;
YYSTYPE yylval;

int main(int argc, char*argv[]){
 int c = yylex();
 while(c!=0 && c!=EOF){
  printf("%lf, %d\n", yylval.val, c);
  c = yylex();
 }
 printf("End of File\n");
}

