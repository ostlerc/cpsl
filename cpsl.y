/* CPSL */
/* cpsl.y */

%{
#include <stdio.h>

int bison_verbose = 0;
%}

%union{
  int   int_val;
  char char_val;
  char* str;
}

/*%start  inputs*/
%start  program

/* keywords */
%token ARRAYSYM ELSESYM IFSYM RECORDSYM TOSYM BEGINSYM ELSEIFSYM OFSYM REPEATSYM TYPESYM CHRSYM ENDSYM
%token ORDSYM RETURNSYM UNTILSYM CONSTSYM FORSYM PREDSYM STOPSYM VARSYM DOSYM FORWARDSYM PROCEDURESYM
%token SUCCSYM WHILESYM DOWNTOSYM FUNCTIONSYM READSYM THENSYM WRITESYM

/* operators */
%token PLUSOSYM NEOSYM SEMIOSYM MINUSOSYM LTOSYM LPARENOSYM STAROSYM LTEOSYM RPARENOSYM FSLASHOSYM
%token GTOSYM LBRACKETOSYM AMPOSYM GTEOSYM RBRACKETOSYM BAROSYM DOTOSYM ASSIGNOSYM TILDEOSYM COMMAOSYM
%token PERCENTOSYM EQOSYM COLONOSYM

/* int */
%token <int_val> INTOSYM
%token <str> IDENTSYM

/* char */
%token CHAROSYM

/* str */
%token STROSYM


%nonassoc ARRAYSYM ELSESYM IFSYM RECORDSYM TOSYM BEGINSYM ELSEIFSYM OFSYM REPEATSYM TYPESYM CHRSYM ENDSYM
%nonassoc ORDSYM RETURNSYM UNTILSYM CONSTSYM FORSYM PREDSYM STOPSYM VARSYM DOSYM FORWARDSYM PROCEDURESYM
%nonassoc SUCCSYM WHILESYM DOWNTOSYM FUNCTIONSYM READSYM THENSYM WRITESYM RBRACKETOSYM LBRACKETOSYM
%nonassoc SEMIOSYM
%nonassoc COMMAOSYM
%nonassoc COLONOSYM

%left BAROSYM
%left AMPOSYM
%left EQOSYM
%left NEOSYM
%left LTEOSYM
%left GTEOSYM
%left LTOSYM
%left GTOSYM
%left PLUSOSYM
%left STAROSYM
%left FSLASHOSYM
%left PERCENTOSYM
%right TILDEOSYM
%right MINUSOSYM
%nonassoc LPARENOSYM
%nonassoc RPARENOSYM

%%

program: constDecl typeDecl varDecl pOrFDecls block DOTOSYM { printf("finished!\n"); }
       ;
block: /*TODO*/
     ;
constDecl: /*empty*/
         | CONSTSYM assignStatements
         ;
typeDecl: /*empty*/
        | TYPESYM typeStatements
        ;
varDecl: /*empty*/
       | VARSYM varStatements
       ;
varStatements: varStatements varStatement
             | varStatement
             ;
varStatement: identList COLONOSYM type SEMIOSYM
            ;
pOrFDecls: /*empty procedures or function decls*/
         | pOrFDecls pOrFDecl
         ;
pOrFDecl: procedureDecl
        | functionDecl
        ;

procedureDecl: PROCEDURESYM IDENTSYM LPARENOSYM formalParameters RPARENOSYM SEMIOSYM FORWARDSYM SEMIOSYM
             | PROCEDURESYM IDENTSYM LPARENOSYM formalParameters RPARENOSYM SEMIOSYM body SEMIOSYM
             ;
functionDecl: FUNCTIONSYM IDENTSYM LPARENOSYM formalParameters RPARENOSYM COLONOSYM type SEMIOSYM FORWARDSYM SEMIOSYM
            | FUNCTIONSYM IDENTSYM LPARENOSYM formalParameters RPARENOSYM COLONOSYM type SEMIOSYM body SEMIOSYM
            ;
formalParameters: /*empty*/
                | formalParameter
                | formalParameters SEMIOSYM formalParameter
                ;
formalParameter: VARSYM identList COLONOSYM type
               | identList COLONOSYM type
               ;
body: constDecl typeDecl varDecl block
    ;

block: BEGINSYM statementSequence ENDSYM
     ;
statementSequence: statementSequence SEMIOSYM statement
                 | statement
                 ;
statement: assignment
         | ifStatement
         | whileStatement
         | repeatStatement
         | forStatement
         | stopStatement
         | returnStatement
         | readStatement
         | writeStatement
         | procedureCall
         | nullStatement
         ;
assignment: lValue ASSIGNOSYM expression
          ;
lValue: IDENTSYM lValueHelper
      ;
lValueHelper: /*empty*/
            | lValueHelper DOTOSYM IDENTSYM
            | lValueHelper LBRACKETOSYM expression RBRACKETOSYM
            ;
lValueList: lValue
          | lValueList COMMAOSYM lValue
          ;
ifStatement: IFSYM expression THENSYM statementSequence elseifStatement elseStatement ENDSYM
           ;
elseifStatement: /*empty*/
               | elseifStatement ELSEIFSYM expression THENSYM statementSequence
               ;
elseStatement: /*empty*/
             | ELSESYM statementSequence
             ;
whileStatement: WHILESYM expression DOSYM statementSequence ENDSYM
              ;
repeatStatement: REPEATSYM statementSequence UNTILSYM expression
               ;
forStatement: FORSYM IDENTSYM ASSIGNOSYM expression TOSYM     expression DOSYM statementSequence ENDSYM
            | FORSYM IDENTSYM ASSIGNOSYM expression DOWNTOSYM expression DOSYM statementSequence ENDSYM
            ;
stopStatement: STOPSYM
             ;
returnStatement: RETURNSYM expression
               | RETURNSYM
               ;
readStatement: READSYM LPARENOSYM lValueList RPARENOSYM
             ;
writeStatement: WRITESYM LPARENOSYM expressionList RPARENOSYM
              ;
procedureCall: IDENTSYM LPARENOSYM RPARENOSYM
             | IDENTSYM LPARENOSYM expressionList RPARENOSYM
             ;
nullStatement: /* empty */
             ;
assignStatements: assignStatement
                | assignStatements assignStatement
                ;
assignStatement: IDENTSYM EQOSYM expression SEMIOSYM { if(bison_verbose) printf("assigned '%s'\n", $1); }
               ;
typeStatements: typeStatement
              | typeStatements typeStatement
              ;
typeStatement: IDENTSYM EQOSYM type SEMIOSYM { if(bison_verbose) printf("type defined '%s'\n", $1); }
             ;
type: simpleType
    | recordType
    | arrayType
    ;
simpleType: IDENTSYM
          ;
recordType: RECORDSYM recordDecls ENDSYM
          ;
recordDecls: /*empty*/
           | recordDecls recordDecl
           ;
recordDecl: identList COLONOSYM type SEMIOSYM
          ;
identList: IDENTSYM commaIdentifiers { if(bison_verbose) printf("id '%s'\n", $1); }
         ;
commaIdentifiers: /*empty*/
                | commaIdentifiers commaIdentifier
                ;
commaIdentifier: COMMAOSYM IDENTSYM { if(bison_verbose) printf("nested comma id '%s'\n", $2); }
               ;

arrayType:  ARRAYSYM LBRACKETOSYM expression COLONOSYM expression RBRACKETOSYM OFSYM type
         ;
expression: INTOSYM
          | CHAROSYM
          | STROSYM
          /*| IDENTSYM covered by lValue*/
          | expression BAROSYM expression
          | expression AMPOSYM expression
          | expression EQOSYM expression
          | expression NEOSYM expression
          | expression LTEOSYM expression
          | expression GTEOSYM expression
          | expression LTOSYM expression
          | expression GTOSYM expression
          | expression PLUSOSYM expression
          | expression MINUSOSYM expression
          | expression STAROSYM expression
          | expression FSLASHOSYM expression
          | expression PERCENTOSYM expression
          | TILDEOSYM expression
          | MINUSOSYM expression
          | LPARENOSYM expression RPARENOSYM
          | IDENTSYM LPARENOSYM RPARENOSYM
          | IDENTSYM LPARENOSYM expressionList RPARENOSYM
          | CHRSYM LPARENOSYM expression RPARENOSYM
          | ORDSYM LPARENOSYM expression RPARENOSYM
          | PREDSYM LPARENOSYM expression RPARENOSYM
          | SUCCSYM LPARENOSYM expression RPARENOSYM
          | lValue
          ;
expressionList: expression
          | expressionList COMMAOSYM expression
          ;
%%

int yyerror(char* s)
{
  extern char *yytext;// defined and maintained in lex.c
  extern int yylineno;// defined and maintained in lex.c
  printf("ERROR! %s %s line: %d\n", s, yytext, yylineno);
  return 0;
}
