%{
#include "hoc1.tab.h"
extern int lineno;
%}
%%
[ \t] { ; }
[0-9]+\.?|[0-9]*\.[0-9]+ { sscanf(yytext, "%lf", &yylval.val); return NUMBER; }
\n { lineno++; return '\n'; }
. { return yytext[0]; }

