typedef struct Symbol { /* symbol table entry */
 char *name;
 short type; /* VAR, BLTIN, UNDEF */
 union {
  double val; /* if VAR */
  double (*ptr)(); /* if BLTIN */
  int (*defn)(); /* FUNCTION, PROCEDURE */
  char *str; /* STRING */
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
extern int eval(), add(), sub(), mul(), hocdiv(), negate(), power();

typedef int (*Inst)();
#define STOP (Inst) 0

extern Inst *progp, *progbase, prog[], *code(Inst f);
extern int assign(), bltin(), varpush(), constpush(), print(), varread();
extern int popstack();
extern int prexpr(), prstr();
extern int gt(), lt(), eq(), ge(), le(), ne(), and(), or(), not();
extern int ifcode(), whilecode(), call(), arg(), argassign();
extern int funcret(), procret();
