/* CPSL */
/* cpsl.y */

%{
#include <stdio.h>
#include <string>
#include <vector>
#include <iostream>

#include "SymbolTable.h"

int bison_verbose = 0;
int yyerror(char* s);
extern "C" int yylex();
%}

%union{
  int int_val;
  std::string *str_val;
  SymbolTable *tableptr;
  Expression *exprptr;
  Symbol *symptr;
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

%type <exprptr> expression
%type <symptr> lValue;

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

program: constDecl typeDecl varDecl pOrFDecls block DOTOSYM { SymbolTable::instance()->end(); }
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
varStatement: identList COLONOSYM type SEMIOSYM { SymbolTable::instance()->create_vars(0); }
            ;
pOrFDecls: /*empty procedures or function decls*/ { SymbolTable::instance()->begin(); }
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
lValue: IDENTSYM lValueHelper { $$ = SymbolTable::instance()->findSymbol($1); }
      ;
lValueHelper: /*empty*/
            | lValueHelper DOTOSYM IDENTSYM
            | lValueHelper LBRACKETOSYM expression RBRACKETOSYM
            ;
lValueList: lValue { SymbolTable::instance()->add_to_lval_list($1); }
          | lValueList COMMAOSYM lValue { SymbolTable::instance()->add_to_lval_list($3); }
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
readStatement: READSYM LPARENOSYM lValueList RPARENOSYM { SymbolTable::instance()->read(); SymbolTable::instance()->checkRegisters(); }
             ;
writeStatement: WRITESYM LPARENOSYM expressionList RPARENOSYM { SymbolTable::instance()->print(); }
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
identList: IDENTSYM { SymbolTable::instance()->add_var($1); if(bison_verbose) printf("id '%s'\n", $1->c_str()); }
         | IDENTSYM commaIdentifiers { SymbolTable::instance()->add_var($1); }
         ;
commaIdentifiers: commaIdentifiers commaIdentifier
                | commaIdentifier
                ;
commaIdentifier: COMMAOSYM IDENTSYM { SymbolTable::instance()->add_var($2); if(bison_verbose) printf("nested comma id '%s'\n", $2->c_str()); }
               ;
arrayType:  ARRAYSYM LBRACKETOSYM expression COLONOSYM expression RBRACKETOSYM OFSYM type
         ;
expression: INTOSYM { $$ = SymbolTable::instance()->expression($1); }
          | CHAROSYM { $$ = SymbolTable::instance()->expression_char($1); }
          | STROSYM { $$ = SymbolTable::instance()->expression_string($1); }
          /*| IDENTSYM covered by lValue*/
          | expression BAROSYM expression { $$ = $1->unimp($3); }
          | expression AMPOSYM expression { $$ = $1->unimp($3); }
          | expression EQOSYM expression { $$ = $1->exec($3, Expression::Eq); }
          | expression NEOSYM expression { $$ = $1->exec($3, Expression::Ne); }
          | expression LTEOSYM expression { $$ = $1->exec($3, Expression::Lte); }
          | expression GTEOSYM expression { $$ = $1->exec($3, Expression::Gte); }
          | expression LTOSYM expression { $$ = $1->exec($3, Expression::Lt); }
          | expression GTOSYM expression { $$ = $1->exec($3, Expression::Gt); }
          | expression PLUSOSYM expression { $$ = $1->exec($3, Expression::Add); }
          | expression MINUSOSYM expression { $$ = $1->exec($3, Expression::Sub); }
          | expression STAROSYM expression { $$ = $1->exec($3, Expression::Mul); }
          | expression FSLASHOSYM expression { $$ = $1->exec($3, Expression::Div); }
          | expression PERCENTOSYM expression { $$ = $1->exec($3, Expression::Mod); }
          | TILDEOSYM expression { $$ = $2->unimp($2); }
          | MINUSOSYM expression %prec UMINUSOSYM { $$ = $2->unimp($2); }
          | LPARENOSYM expression RPARENOSYM { $$ = $2; }
          | IDENTSYM LPARENOSYM RPARENOSYM { $$ = SymbolTable::instance()->unimp(); }
          | IDENTSYM LPARENOSYM expressionList RPARENOSYM { $$ = SymbolTable::instance()->unimp(); }
          | CHRSYM LPARENOSYM expression RPARENOSYM { $$ = $3->unimp($3); }
          | ORDSYM LPARENOSYM expression RPARENOSYM { $$ = $3->unimp($3); }
          | PREDSYM LPARENOSYM expression RPARENOSYM { $$ = $3->unimp($3); }
          | SUCCSYM LPARENOSYM expression RPARENOSYM { $$ = $3->unimp($3); }
          | lValue { $$ = SymbolTable::instance()->lValue($1); }
          ;
expressionList: expression { SymbolTable::instance()->add_to_expr_list($1); }
          | expressionList COMMAOSYM expression { SymbolTable::instance()->add_to_expr_list($3); }
          ;
%%

int yyerror(char* s)
{
  extern char *yytext;// defined and maintained in lex.cpp
  extern int yylineno;// defined and maintained in lex.cpp
  printf("ERROR! %s %s line: %d\n", s, yytext, yylineno);
  return 0;
}
