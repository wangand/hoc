#include <stdio.h>
#include <ctype.h>
#include "hoc.tab.h"
#include "hoc.h"
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

if(isalpha(c)){
 struct Symbol *s;
 char sbuf[100], *p = sbuf;
 do{
   *p++ = c;
 }while((c=getchar()) != EOF && isalnum(c) );
 ungetc(c, stdin);
 *p = '\0';
 if((s=lookup(sbuf)) == 0){
  s = install(sbuf, UNDEF, 0.0);
 }
 yylval.sym = s;
 return s->type == UNDEF ? VAR : s->type;
}

 if (c=='\n'){
  lineno++;
 }
 return c;
}

