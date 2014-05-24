/* CPSL */
/* cpsl.y */

%{
#include <stdio.h>
#include <string>
#include <list>

std::string *expression(int);
std::string *expression(std::string*);
int bison_verbose = 0;
int yyerror(char* s);
extern "C" int yylex();
%}

%union{
  int   int_val;
  std::string *str_val;
  std::list<std::string> *var_list;
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

%token <int_val> INTOSYM
%token <str_val> CHAROSYM
%token <str_val> STROSYM
%token <str_val> IDENTSYM

%type <str_val> expression
%type <var_list> identList

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
%left MINUSOSYM
%left STAROSYM
%left FSLASHOSYM
%left PERCENTOSYM
%right TILDEOSYM
%right UMINUSOSYM
%nonassoc LPARENOSYM
%nonassoc RPARENOSYM

%%

program: constDecl typeDecl varDecl pOrFDecls block DOTOSYM { printf("finished!\n"); }
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
assignStatement: IDENTSYM EQOSYM expression SEMIOSYM { if(bison_verbose) printf("assigned '%s'\n", $1->c_str()); }
               ;
typeStatements: typeStatement
              | typeStatements typeStatement
              ;
typeStatement: IDENTSYM EQOSYM type SEMIOSYM { if(bison_verbose) printf("type defined '%s'\n", $1->c_str()); }
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
identList: IDENTSYM commaIdentifiers { if(bison_verbose) printf("id '%s'\n", $1->c_str()); }
         ;
commaIdentifiers: /*empty*/
                | commaIdentifiers commaIdentifier
                ;
commaIdentifier: COMMAOSYM IDENTSYM { $$ = addToList($1); if(bison_verbose) printf("nested comma id '%s'\n", $2->c_str()); }
               ;
arrayType:  ARRAYSYM LBRACKETOSYM expression COLONOSYM expression RBRACKETOSYM OFSYM type
         ;
expression: INTOSYM { $$ = expression($1); }
          | CHAROSYM { $$ = expression($1); }
          | STROSYM { $$ = expression($1); }
          /*| IDENTSYM covered by lValue*/
          | expression BAROSYM expression { $$ = new std::string("<err>"); }
          | expression AMPOSYM expression { $$ = new std::string("<err>"); }
          | expression EQOSYM expression { $$ = new std::string("<err>"); }
          | expression NEOSYM expression { $$ = new std::string("<err>"); }
          | expression LTEOSYM expression { $$ = new std::string("<err>"); }
          | expression GTEOSYM expression { $$ = new std::string("<err>"); }
          | expression LTOSYM expression { $$ = new std::string("<err>"); }
          | expression GTOSYM expression { $$ = new std::string("<err>"); }
          | expression PLUSOSYM expression { $$ = new std::string("<err>"); }
          | expression MINUSOSYM expression { $$ = new std::string("<err>"); }
          | expression STAROSYM expression { $$ = new std::string("<err>"); }
          | expression FSLASHOSYM expression { $$ = new std::string("<err>"); }
          | expression PERCENTOSYM expression { $$ = new std::string("<err>"); }
          | TILDEOSYM expression { $$ = new std::string("<err>"); }
          | MINUSOSYM expression %prec UMINUSOSYM { $$ = new std::string("<err>"); }
          | LPARENOSYM expression RPARENOSYM { $$ = new std::string("<err>"); }
          | IDENTSYM LPARENOSYM RPARENOSYM { $$ = new std::string("<err>"); }
          | IDENTSYM LPARENOSYM expressionList RPARENOSYM { $$ = new std::string("<err>"); }
          | CHRSYM LPARENOSYM expression RPARENOSYM { $$ = new std::string("<err>"); }
          | ORDSYM LPARENOSYM expression RPARENOSYM { $$ = new std::string("<err>"); }
          | PREDSYM LPARENOSYM expression RPARENOSYM { $$ = new std::string("<err>"); }
          | SUCCSYM LPARENOSYM expression RPARENOSYM { $$ = new std::string("<err>"); }
          | lValue { $$ = new std::string("<err>"); }
          ;
expressionList: expression
          | expressionList COMMAOSYM expression
          ;
%%

int yyerror(char* s)
{
  extern char *yytext;// defined and maintained in lex.cpp
  extern int yylineno;// defined and maintained in lex.cpp
  printf("ERROR! %s %s line: %d\n", s, yytext, yylineno);
  return 0;
}
