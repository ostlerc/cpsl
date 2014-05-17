/* Mini Calculator */
/* calc.lex */

%{
#include <stdio.h>
#include "calc.h"
%}

digit		[0-9]
int_const	{digit}+
terminal    ;
parenl      \(
parenr      \)

%%

{int_const}	{ yylval.int_val = atoi(yytext); return INTEGER_LITERAL; }
"\+"		{ yylval.op_val = strdup(yytext); return PLUS; }
"\*"		{ yylval.op_val = strdup(yytext); return MULT; }
{terminal}  { return TERM; }
{parenl}    { return PARENL; }
{parenr}    { return PARENR; }

[ \t]*		{}
[\n]		{ yylineno++;	}

.		{ fprintf(stderr, "SCANNER %s", yytext); exit(1);	}

