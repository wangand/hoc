#include <stdio.h>
#include <string.h>
extern char *progname;
extern int lineno;
extern char *infile;
extern FILE *fin;
extern char **gargv;
extern int gargc;

int moreinput(){
 if(gargc-- <= 0){
  return 0;
 }
 if (fin && fin != stdin){
  fclose(fin);
 }
 infile = *gargv++;
 lineno = 1;
 if(strcmp(infile, "-") == 0){
  fin = stdin;
  infile = 0;
 }
 else if((fin=fopen(infile, "r")) == NULL){
  fprintf(stderr, "%s: can't open %s\n", progname, infile);
  return moreinput();
 }
 return 1;
}

