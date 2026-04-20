#include <stdio.h>
#include <ctype.h>
/*
NOTE
because lexer.c requires hoc.tab.h and hoc.tab.h requires hoc.h
if hoc.h is not included above hoc.tab.h there will be an
error in the union statement in hoc.y
*/
#include "hoc.h"
#include "hoc.tab.h"
extern int lineno;

typedef enum yytokentype yyenum;
yyenum follow(char expect, yyenum ifyes, yyenum ifno){
 int c = getchar();
 if (c == expect){
  return ifyes;
 }
 ungetc(c,stdin);
 return ifno;
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

