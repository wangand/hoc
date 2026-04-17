typedef struct Symbol { /* symbol table entry */
 char *name;
 short type; /* VAR, BLTIN, UNDEF */
 union {
  double val; /* if VAR */
  double (*ptr)(); /* if BLTIN */
 } u;
 struct Symbol *next;
} Symbol;
Symbol *install(char *s, int t, double d), *lookup(char *s);

extern void execerror(char* s, char*t);

typedef union Datum {
 double val;
 Symbol *sym;
} Datum;
extern Datum pop();

typedef int (*Inst)();
#define STOP (Inst) 0

extern Inst prog[];
extern int eval(), add(), sub(), mul(), div(), negate(), power();
extern int assign(), bltin(), varpush(), constpush(), negate(), print();
