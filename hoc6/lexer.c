#include <stdio.h>
#include <ctype.h>
#include <string.h>
/*
NOTE
because lexer.c requires hoc.tab.h and hoc.tab.h requires hoc.h
if hoc.h is not included above hoc.tab.h there will be an
error in the union statement in hoc.y
*/
#include "hoc.h"
#include "hoc.tab.h"
extern int lineno;
extern char *emalloc(unsigned n);

typedef enum yytokentype yyenum;
yyenum follow(char expect, yyenum ifyes, yyenum ifno){
 int c = getchar();
 if (c == expect){
  return ifyes;
 }
 ungetc(c,stdin);
 return ifno;
}

int backslash(int c){
 static char transtab[] = "b\bf\fn\nr\rt\t";
 if(c!= '\\'){
  return c;
 }
 c = getc(stdin);
 if(islower(c) && strchr(transtab,c)){
  return strchr(transtab, c)[1];
 }
 return c;
}

int yylex(){
 int c = getchar ();
 /* Skip white space. */
 while (c == ' ' || c == '\t')
  c = getchar ();
 if (c==EOF){
  return 0;
 }
 if (c == '.' || isdigit(c)){
  double d;
  ungetc(c,stdin);
  scanf("%lf", &d);
  yylval.sym = install("", NUMBER, d);
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
if(c=='$'){ /* argument? */
 int n = 0;
 while(isdigit(c=getc(stdin))){
  n = 10*n+c-'0';
 }
 ungetc(c,stdin);
 if(n==0){
  execerror("strange $...",(char *)0);
 }
 yylval.narg = n;
 return ARG;
}
if(c=='"'){ /* quoted string */
 char sbuf[100], *p;
 for(p=sbuf; (c=getc(stdin)) != '"'; p++){
  if(c == '\n' || c==EOF){
   execerror("missing quote", "");
  }
  if(p >= sbuf + sizeof(sbuf)-1){
   *p = '\0';
   execerror("string too long", sbuf);
  }
  *p = backslash(c);
 }
 *p = 0;
 yylval.sym = (Symbol *)emalloc(strlen(sbuf)+1);
 strcpy((char *)yylval.sym, sbuf);
 return STRING;
}
 switch(c){
  case '>': return follow('=', GE, GT);
  case '<': return follow('=', LE, LT);
  case '=': return follow('=', EQ, '=');
  case '!': return follow('=', NE, NOT);
  case '|': return follow('|', OR, '|');
  case '&': return follow('&', AND, '&');
  case '\n': lineno++; return '\n';
  default: return c;
 }
}

