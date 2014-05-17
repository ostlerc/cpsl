/* CPSL */
/* cpsl.y */

%{
#include <stdio.h>

int bison_verbose = 1;
%}

%union{
  int   int_val;
  char char_val;
}

%start  inputs

/* keywords */
%token ARRAYSYM ELSESYM IFSYM RECORDSYM TOSYM BEGINSYM ELSEIFSYM OFSYM REPEATSYM TYPESYM CHRSYM ENDSYM
%token ORDSYM RETURNSYM UNTILSYM CONSTSYM FORSYM PREDSYM STOPSYM VARSYM DOSYM FORWARDSYM PROCEDURESYM
%token SUCCSYM WHILESYM DOWNTOSYM FUNCTIONSYM READSYM THENSYM WRITESYM IDENTSYM

/* operators */
%token PLUSOSYM NEOSYM SEMIOSYM MINUSOSYM LTOSYM LPARENOSYM STAROSYM LTEOSYM RPARENOSYM FSLASHOSYM
%token GTOSYM LBRACKETOSYM AMPOSYM GTEOSYM RBRACKETOSYM BAROSYM DOTOSYM ASSIGNOSYM TILDEOSYM COMMAOSYM
%token PERCENTOSYM EQOSYM COLONOSYM 

/* int */
%token <int_val> INTOSYM 

/* char */
%token CHAROSYM 

/* str */
%token STROSYM

%type <int_val>     exp
%nonassoc   SEMIOSYM
%nonassoc LTOSYM
%nonassoc LTEOSYM
%nonassoc GTOSYM
%nonassoc GTEOSYM
%left   PLUSOSYM
%left   STAROSYM

%nonassoc LPARENOSYM
%nonassoc RPARENOSYM

%%

input:  /*empty*/
        | exp   { printf("Result: %d\n", $1); }
        ;

inputs:  input
        | inputs SEMIOSYM input
        | SEMIOSYM
        ;

exp:    INTOSYM { $$ = $1; if(bison_verbose) printf("literal %d\n", $1); }
        | exp PLUSOSYM exp  { $$ = $1 + $3; if(bison_verbose) printf("plus %d\n", $$); }
        | exp STAROSYM exp  { $$ = $1 * $3; if(bison_verbose) printf("mult %d\n", $$); }
        | exp LTOSYM exp { $$ = $1 < $3; if(bison_verbose) printf("less than %d\n", $$); }
        | exp GTOSYM exp { $$ = $1 > $3; if(bison_verbose) printf("greater than %d\n", $$); }
        | exp GTEOSYM exp { $$ = $1 >= $3; if(bison_verbose) printf("greater than equal %d\n", $$); }
        | exp LTEOSYM exp { $$ = $1 <= $3; if(bison_verbose) printf("less than equal %d\n", $$); }
        | LPARENOSYM exp RPARENOSYM { $$ = $2; }
        ;

%%

int yyerror(char* s)
{
  extern char *yytext;// defined and maintained in lex.c
  printf("ERROR! %s %s\n", s, yytext);
  return 0;
}
