#include <stdio.h>
#include <ctype.h>
#include "hoc.tab.h"
extern int lineno;

int yylex(){
 int c = getchar ();
 /* Skip white space. */
 while (c == ' ' || c == '\t')
  c = getchar ();
 if (c==EOF){
  return 0;
 }
 if (c == '.' || isdigit(c)){
 ungetc(c,stdin);
 scanf("%lf", &yylval.val);
 return NUMBER;
}
 if (c=='\n'){
  lineno++;
 }
 return c;
}

