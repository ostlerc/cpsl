    int verbose = 0;
    char* t = 0; //used for strtol end pointer

%{
#include "cpsl.h"
%}

%%
\n { ++yylineno; if(verbose) printf("Found newline\n"); } /*after regex*/
[[:blank:]] { if(verbose) printf("ignoring space\n"); }
(array|ARRAY) { if(verbose) printf("keyword(%s)\n", yytext); return ARRAYSYM; }
(else|ELSE) { if(verbose) printf("keyword(%s)\n", yytext); return ELSESYM; }
(if|IF) { if(verbose) printf("keyword(%s)\n", yytext); return IFSYM; }
(record|RECORD) { if(verbose) printf("keyword(%s)\n", yytext); return RECORDSYM; }
(to|TO) { if(verbose) printf("keyword(%s)\n", yytext); return TOSYM; }
(begin|BEGIN) { if(verbose) printf("keyword(%s)\n", yytext); return BEGINSYM; }
(elseif|ELSEIF) { if(verbose) printf("keyword(%s)\n", yytext); return ELSEIFSYM; }
(of|OF) { if(verbose) printf("keyword(%s)\n", yytext); return OFSYM; }
(repeat|REPEAT) { if(verbose) printf("keyword(%s)\n", yytext); return REPEATSYM; }
(type|TYPE) { if(verbose) printf("keyword(%s)\n", yytext); return TYPESYM; }
(chr|CHR) { if(verbose) printf("keyword(%s)\n", yytext); return CHRSYM; }
(end|END) { if(verbose) printf("keyword(%s)\n", yytext); return ENDSYM; }
(ord|ORD) { if(verbose) printf("keyword(%s)\n", yytext); return ORDSYM; }
(return|RETURN) { if(verbose) printf("keyword(%s)\n", yytext); return RETURNSYM; }
(until|UNTIL) { if(verbose) printf("keyword(%s)\n", yytext); return UNTILSYM; }
(const|CONST) { if(verbose) printf("keyword(%s)\n", yytext); return CONSTSYM; }
(for|FOR) { if(verbose) printf("keyword(%s)\n", yytext); return FORSYM; }
(pred|PRED) { if(verbose) printf("keyword(%s)\n", yytext); return PREDSYM; }
(stop|STOP) { if(verbose) printf("keyword(%s)\n", yytext); return STOPSYM; }
(var|VAR) { if(verbose) printf("keyword(%s)\n", yytext); return VARSYM; }
(do|DO) { if(verbose) printf("keyword(%s)\n", yytext); return DOSYM; }
(forward|FORWARD) { if(verbose) printf("keyword(%s)\n", yytext); return FORWARDSYM; }
(procedure|PROCEDURE) { if(verbose) printf("keyword(%s)\n", yytext); return PROCEDURESYM; }
(succ|SUCC) { if(verbose) printf("keyword(%s)\n", yytext); return SUCCSYM; }
(while|WHILE) { if(verbose) printf("keyword(%s)\n", yytext); return WHILESYM; }
(downto|DOWNTO) { if(verbose) printf("keyword(%s)\n", yytext); return DOWNTOSYM; }
(function|FUNCTION) { if(verbose) printf("keyword(%s)\n", yytext); return FUNCTIONSYM; }
(read|READ) { if(verbose) printf("keyword(%s)\n", yytext); return READSYM; }
(then|THEN) { if(verbose) printf("keyword(%s)\n", yytext); return THENSYM; }
(write|WRITE) { if(verbose) printf("keyword(%s)\n", yytext); return WRITESYM; }
[[:alpha:]][[:alnum:]_]* { yylval.str = strdup(yytext); if(verbose) printf("identifier(%s)\n", yytext); return IDENTSYM; }
\+ { if(verbose) printf("operator_delimiter(%s)\n", yytext); return PLUSOSYM; }
\<\> { if(verbose) printf("operator_delimiter(%s)\n", yytext); return NEOSYM; }
; { if(verbose) printf("operator_delimiter(%s)\n", yytext); return SEMIOSYM; }
- { if(verbose) printf("operator_delimiter(%s)\n", yytext); return MINUSOSYM; }
\< { if(verbose) printf("operator_delimiter(%s)\n", yytext); return LTOSYM; }
\( { if(verbose) printf("operator_delimiter(%s)\n", yytext); return LPARENOSYM; }
\* { if(verbose) printf("operator_delimiter(%s)\n", yytext); return STAROSYM; }
\<= { if(verbose) printf("operator_delimiter(%s)\n", yytext); return LTEOSYM; }
\) { if(verbose) printf("operator_delimiter(%s)\n", yytext); return RPARENOSYM; }
\/ { if(verbose) printf("operator_delimiter(%s)\n", yytext); return FSLASHOSYM; }
\> { if(verbose) printf("operator_delimiter(%s)\n", yytext); return GTOSYM; }
\[ { if(verbose) printf("operator_delimiter(%s)\n", yytext); return LBRACKETOSYM; }
\& { if(verbose) printf("operator_delimiter(%s)\n", yytext); return AMPOSYM; }
\>= { if(verbose) printf("operator_delimiter(%s)\n", yytext); return GTEOSYM; }
\] { if(verbose) printf("operator_delimiter(%s)\n", yytext); return RBRACKETOSYM; }
\| { if(verbose) printf("operator_delimiter(%s)\n", yytext); return BAROSYM; }
\. { if(verbose) printf("operator_delimiter(%s)\n", yytext); return DOTOSYM; }
:= { if(verbose) printf("operator_delimiter(%s)\n", yytext); return ASSIGNOSYM; }
~ { if(verbose) printf("operator_delimiter(%s)\n", yytext); return TILDEOSYM; }
, { if(verbose) printf("operator_delimiter(%s)\n", yytext); return COMMAOSYM; }
\% { if(verbose) printf("operator_delimiter(%s)\n", yytext); return PERCENTOSYM; }
\= { if(verbose) printf("operator_delimiter(%s)\n", yytext); return EQOSYM; }
\: { if(verbose) printf("operator_delimiter(%s)\n", yytext); return COLONOSYM; }
0x[[:digit:]a-fA-F]+ { yylval.int_val = strtol(yytext, &t, 16); if(verbose) printf("hex(%s)\n", yytext); return INTOSYM; }
0[^x][0-7]+ { yylval.int_val = strtol(yytext, &t, 8); if(verbose) printf("octal(%s)\n", yytext); return INTOSYM; }
[[:digit:]]+ { yylval.int_val = atoi(yytext); if(verbose) printf("integer(%s)\n", yytext); return INTOSYM; }
'(\\[nrbtf]|[[:alpha:] ])' { if(verbose) printf("character(%s)\n", yytext); return CHAROSYM; }
\"[^\"\n]*\" { if(verbose) printf("string(%s)\n", yytext); return STROSYM; }
\$.*$ { if(verbose) printf("comment(%s)\n", yytext); }
. { fprintf(stderr, "Incorrect syntax on line %d '%s'\n", yylineno, yytext); REJECT; }
%%
