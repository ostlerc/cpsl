    int verbose = 0;
    char* t = 0; //used for strtol end pointer

%{
#include "cpsl.h"
%}

%%
\n { ++yylineno; if(verbose) printf("Found newline\n"); } /*after regex*/
[[:blank:]] { if(verbose) printf("ignoring space\n"); }
(array|ARRAY) { printf("keyword(%s)\n", yytext); return ARRAYSYM; }
(else|ELSE) { printf("keyword(%s)\n", yytext); return ELSESYM; }
(if|IF) { printf("keyword(%s)\n", yytext); return IFSYM; }
(record|RECORD) { printf("keyword(%s)\n", yytext); return RECORDSYM; }
(to|TO) { printf("keyword(%s)\n", yytext); return TOSYM; }
(begin|BEGIN) { printf("keyword(%s)\n", yytext); return BEGINSYM; }
(elseif|ELSEIF) { printf("keyword(%s)\n", yytext); return ELSEIFSYM; }
(of|OF) { printf("keyword(%s)\n", yytext); return OFSYM; }
(repeat|REPEAT) { printf("keyword(%s)\n", yytext); return REPEATSYM; }
(type|TYPE) { printf("keyword(%s)\n", yytext); return TYPESYM; }
(chr|CHR) { printf("keyword(%s)\n", yytext); return CHRSYM; }
(end|END) { printf("keyword(%s)\n", yytext); return ENDSYM; }
(ord|ORD) { printf("keyword(%s)\n", yytext); return ORDSYM; }
(return|RETURN) { printf("keyword(%s)\n", yytext); return RETURNSYM; }
(until|UNTIL) { printf("keyword(%s)\n", yytext); return UNTILSYM; }
(const|CONST) { printf("keyword(%s)\n", yytext); return CONSTSYM; }
(for|FOR) { printf("keyword(%s)\n", yytext); return FORSYM; }
(pred|PRED) { printf("keyword(%s)\n", yytext); return PREDSYM; }
(stop|STOP) { printf("keyword(%s)\n", yytext); return STOPSYM; }
(var|VAR) { printf("keyword(%s)\n", yytext); return VARSYM; }
(do|DO) { printf("keyword(%s)\n", yytext); return DOSYM; }
(forward|FORWARD) { printf("keyword(%s)\n", yytext); return FORWARDSYM; }
(procedure|PROCEDURE) { printf("keyword(%s)\n", yytext); return PROCEDURESYM; }
(succ|SUCC) { printf("keyword(%s)\n", yytext); return SUCCSYM; }
(while|WHILE) { printf("keyword(%s)\n", yytext); return WHILESYM; }
(downto|DOWNTO) { printf("keyword(%s)\n", yytext); return DOWNTOSYM; }
(function|FUNCTION) { printf("keyword(%s)\n", yytext); return FUNCTIONSYM; }
(read|READ) { printf("keyword(%s)\n", yytext); return READSYM; }
(then|THEN) { printf("keyword(%s)\n", yytext); return THENSYM; }
(write|WRITE) { printf("keyword(%s)\n", yytext); return WRITESYM; }
[[:alpha:]][[:alnum:]_]* { printf("identifier(%s)\n", yytext); return IDENTSYM; }
\+ { printf("operator_delimiter(%s)\n", yytext); return PLUSOSYM; }
\<\> { printf("operator_delimiter(%s)\n", yytext); return NEOSYM; }
; { printf("operator_delimiter(%s)\n", yytext); return SEMIOSYM; }
- { printf("operator_delimiter(%s)\n", yytext); return MINUSOSYM; }
\< { printf("operator_delimiter(%s)\n", yytext); return LTOSYM; }
\( { printf("operator_delimiter(%s)\n", yytext); return LPARENOSYM; }
\* { printf("operator_delimiter(%s)\n", yytext); return STAROSYM; }
\<= { printf("operator_delimiter(%s)\n", yytext); return LTEOSYM; }
\) { printf("operator_delimiter(%s)\n", yytext); return RPARENOSYM; }
\/ { printf("operator_delimiter(%s)\n", yytext); return FSLASHOSYM; }
\> { printf("operator_delimiter(%s)\n", yytext); return GTOSYM; }
\[ { printf("operator_delimiter(%s)\n", yytext); return LBRACKETOSYM; }
\& { printf("operator_delimiter(%s)\n", yytext); return AMPOSYM; }
\>= { printf("operator_delimiter(%s)\n", yytext); return GTEOSYM; }
\] { printf("operator_delimiter(%s)\n", yytext); return RBRACKETOSYM; }
\| { printf("operator_delimiter(%s)\n", yytext); return BAROSYM; }
\. { printf("operator_delimiter(%s)\n", yytext); return DOTOSYM; }
:= { printf("operator_delimiter(%s)\n", yytext); return ASSIGNOSYM; }
~ { printf("operator_delimiter(%s)\n", yytext); return TILDEOSYM; }
, { printf("operator_delimiter(%s)\n", yytext); return COMMAOSYM; }
\% { printf("operator_delimiter(%s)\n", yytext); return PERCENTOSYM; }
\= { printf("operator_delimiter(%s)\n", yytext); return EQOSYM; }
\: { printf("operator_delimiter(%s)\n", yytext); return COLONOSYM; }
0x[[:digit:]a-fA-F]+ { yylval.int_val = strtol(yytext, &t, 16); printf("hex(%s)\n", yytext); return INTOSYM; }
0[^x][0-7]+ { yylval.int_val = strtol(yytext, &t, 8); printf("octal(%s)\n", yytext); return INTOSYM; }
[[:digit:]]+ { yylval.int_val = atoi(yytext); printf("integer(%s)\n", yytext); return INTOSYM; }
'\\*.+' { printf("character(%s)\n", yytext); return CHAROSYM; }
\"[^\"\n]*\" { printf("string(%s)\n", yytext); return STROSYM; }
\$.*$ { printf("comment(%s)\n", yytext); }
. { fprintf(stderr, "Incorrect syntax on line %d '%s'\n", yylineno, yytext); REJECT; }
%%
