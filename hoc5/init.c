#include "hoc.h"
#include "hoc.tab.h"
#include <math.h>

extern double Log(), Log10(), Exp(), Sqrt(), integer();

struct HocConstant{
 char *name;
 double cval;
};
static struct HocConstant consts[6] = {
 { "PI", 3.141592},
 { "E", 2.71828},
 { "GAMMA", 0.5772},
 { "DEG", 57.2957795},
 { "PHI", 1.61803},
 { 0, 0}
};

struct HocBuiltin{
 char *name;
 double (*func)();
};
static struct HocBuiltin builtins[10] = {
 { "sin", sin },
 { "cos", cos },
 { "atan", atan},
 { "log", Log},
 { "log10", Log10},
 { "exp", Exp},
 { "sqrt", Sqrt},
 { "int", integer},
 { "abs", fabs},
 { 0, 0}
};

struct HocKeywords{
 char *name;
 int kval;
};
static struct HocKeywords keywords[5] = {
 { "if", IF },
 { "else", ELSE },
 { "while", WHILE },
 { "print", PRINT},
 { 0, 0 }
};

void init(){
 int i;
 Symbol *s;

 for(i=0;consts[i].name;i++){
  install(consts[i].name, VAR, consts[i].cval);
 }
 for(i=0;builtins[i].name;i++){
  s = install(builtins[i].name, BLTIN, 0.0);
  s->u.ptr = builtins[i].func;
 }
 for(i=0; keywords[i].name; i++){
  install(keywords[i].name, keywords[i].kval, 0.0);
 }
}
