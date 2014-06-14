/* CPSL */
/* cpsl.y */

%{
#include <stdio.h>
#include <string>
#include <vector>
#include <iostream>

#include "SymbolTable.h"
#include "Type.h"
#include "Log.h"
#include "Symbol.h"

int bison_verbose = 0;
int yyerror(const char* s);
extern int yylineno;
extern Log* log;
extern "C" int yylex();
%}

%union{
  int int_val;
  std::string *str_val;
  Expression *exprptr;
  Symbol *symptr;
  ExprList *expr_list;
  StrList *str_list;
  Parameters *paramptr;
  ParamList *param_list;
  Type *typeexpr;
  SymList *sym_list;
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
%type <exprptr> lValue;
%type <expr_list> lValueList;
%type <typeexpr> type;
%type <typeexpr> arrayType;
/*%type <typeexpr> recordType;*/
%type <paramptr> formalParameter;
%type <param_list> formalParameters;
%type <str_val> whileHead; /*return start label*/
%type <str_val> whileExpr; /*return end label*/
%type <str_val> ifExpr;    /*return end label*/
%type <str_val> elseifExpr;
%type <exprptr> forAssign;
%type <str_val> forExpr;
%type <str_val> repeatHead;
%type <param_list> procedureParams;
%type <param_list> functionParams;
%type <expr_list> expressionList;
%type <str_val> commaIdentifier;
%type <str_list> commaIdentifiers;
%type <str_list> identList;

%nonassoc ARRAYSYM ELSESYM IFSYM RECORDSYM TOSYM BEGINSYM ELSEIFSYM OFSYM REPEATSYM TYPESYM CHRSYM ENDSYM
%nonassoc ORDSYM RETURNSYM UNTILSYM CONSTSYM FORSYM PREDSYM STOPSYM VARSYM DOSYM FORWARDSYM PROCEDURESYM
%nonassoc SUCCSYM WHILESYM DOWNTOSYM FUNCTIONSYM READSYM THENSYM WRITESYM RBRACKETOSYM LBRACKETOSYM
%nonassoc SEMIOSYM
%nonassoc COMMAOSYM
%nonassoc COLONOSYM

%right TILDEOSYM
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
%right UMINUSOSYM
%nonassoc LPARENOSYM
%nonassoc RPARENOSYM

%%

program: constDecl typeDecl varDecl pOrFDecls programBlock DOTOSYM { SymbolTable::instance()->end(); }
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
varStatement: identList COLONOSYM type SEMIOSYM { SymbolTable::instance()->create_vars($3, $1->list()); }
            ;
pOrFDecls: /*empty procedures or function decls*/
         | pOrFDecls pOrFDecl
         ;
pOrFDecl: procedureDecl
        | functionDecl
        ;
procedureDecl: PROCEDURESYM IDENTSYM LPARENOSYM formalParameters RPARENOSYM FORWARDSYM SEMIOSYM { SymbolTable::instance()->forwardProc(*$2,$4->list()); }
             | PROCEDURESYM procedureParams body SEMIOSYM { SymbolTable::instance()->endProcedure($2->list()); }
             ;
procedureParams: IDENTSYM LPARENOSYM formalParameters RPARENOSYM SEMIOSYM { $$ = $3; SymbolTable::instance()->procedureParams(*$1,$3->list()); }
               ;
functionDecl: FUNCTIONSYM IDENTSYM LPARENOSYM formalParameters RPARENOSYM COLONOSYM type SEMIOSYM FORWARDSYM SEMIOSYM { SymbolTable::instance()->forwardFunc(*$2,$4->list(), $7); }
            | FUNCTIONSYM functionParams body SEMIOSYM { SymbolTable::instance()->endProcedure($2->list()); }
            ;
functionParams: IDENTSYM LPARENOSYM formalParameters RPARENOSYM COLONOSYM type SEMIOSYM { $$ = $3; SymbolTable::instance()->procedureParams(*$1,$3->list(), $6); }
              ;
formalParameters: /*empty*/ { $$ = new ParamList; }
                | formalParameter { $$ = new ParamList(*$1); }
                | formalParameters SEMIOSYM formalParameter { $$ = $1->add(*$3); }
                ;
formalParameter: VARSYM identList COLONOSYM type { $$ = new Parameters($2->list(), $4); }
               | identList COLONOSYM type { $$ = new Parameters($1->list(), $3); }
               ;
body: constDecl typeDecl varDecl block
    ;
programHead: BEGINSYM { SymbolTable::instance()->begin(); }
           ;
programBlock: programHead statementSequence ENDSYM
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
assignment: lValue ASSIGNOSYM expression { SymbolTable::instance()->assign($1, $3); }
          ;
lValue: IDENTSYM { $$ = new Expression(SymbolTable::instance()->findSymbol(*$1)); }
      | IDENTSYM DOTOSYM IDENTSYM { $$ = NULL; }
      | IDENTSYM LBRACKETOSYM expression RBRACKETOSYM { $$ = SymbolTable::instance()->arrayIndex(*$1, $3); }
      ;
lValueList: lValue { $$ = new ExprList($1); }
          | lValueList COMMAOSYM lValue { $$ = $1->add($3); }
          ;
ifStatement: ifCondition elseifStatement elseStatement ENDSYM { SymbolTable::instance()->ifStatement(); }
           ;
ifCondition: ifExpr statementSequence { SymbolTable::instance()->ifCondition(*$1); }
           ;
ifExpr: IFSYM expression THENSYM { $$ = SymbolTable::instance()->ifExpr($2); }
      ;
elseifStatement: /*empty*/
               | elseifExpr statementSequence { SymbolTable::instance()->elseifStatement(*$1); }
               ;
elseifExpr: elseifStatement ELSEIFSYM expression THENSYM { $$ = SymbolTable::instance()->elseifExpr($3); }
          ;
elseStatement: /*empty*/
             | ELSESYM statementSequence
             ;
whileStatement: whileHead whileExpr statementSequence ENDSYM { SymbolTable::instance()->whileStatement(*$1, *$2); }
              ;
whileExpr: expression DOSYM { $$ = SymbolTable::instance()->whileExpr($1); }
         ;
whileHead: WHILESYM { $$ = SymbolTable::instance()->whileStart(); }
         ;
repeatStatement: repeatHead statementSequence UNTILSYM expression { SymbolTable::instance()->repeatStatement($1, $4); }
               ;
repeatHead: REPEATSYM { $$ = SymbolTable::instance()->repeatHead(); }
forStatement: FORSYM forExpr DOSYM statementSequence ENDSYM { SymbolTable::instance()->forStatement(*$2); }
            ;
forExpr: forAssign TOSYM     expression { $$ = SymbolTable::instance()->forExpr($1, $3, Expression::Succ); }
       | forAssign DOWNTOSYM expression { $$ = SymbolTable::instance()->forExpr($1, $3, Expression::Pred); }
       ;
forAssign: IDENTSYM ASSIGNOSYM expression { $$ = SymbolTable::instance()->assign(*$1, $3); }
         ;
stopStatement: STOPSYM { SymbolTable::instance()->stop(); }
             ;
returnStatement: RETURNSYM expression { SymbolTable::instance()->_return($2); }
               | RETURNSYM { SymbolTable::instance()->_return(); }
               ;
readStatement: READSYM LPARENOSYM lValueList RPARENOSYM { SymbolTable::instance()->read($3->list()); }
             ;
writeStatement: WRITESYM LPARENOSYM expressionList RPARENOSYM { SymbolTable::instance()->print($3->list()); }
              ;
procedureCall: IDENTSYM LPARENOSYM RPARENOSYM { SymbolTable::instance()->callProc(*$1); }
             | IDENTSYM LPARENOSYM expressionList RPARENOSYM { SymbolTable::instance()->callProc(*$1, $3->list()); }
             ;
nullStatement: /* empty */
             ;
assignStatements: assignStatement
                | assignStatements assignStatement
                ;
assignStatement: IDENTSYM EQOSYM expression SEMIOSYM { SymbolTable::instance()->declare_const(*$1, $3); }
               ;
typeStatements: typeStatement
              | typeStatements typeStatement
              ;
typeStatement: typeHead EQOSYM type SEMIOSYM
             ;
typeHead: IDENTSYM { SymbolTable::instance()->startTypeDeclare(*$1); }
        ;
type: IDENTSYM { $$ = SymbolTable::instance()->findType(*$1); }
    | recordType { $$ = Type::typeRecord(); }
    | arrayType  { $$ = $1; }
    ;
recordType: RECORDSYM recordDecls ENDSYM
          ;
recordDecls: /*empty*/
           | recordDecls recordDecl
           ;
recordDecl: identList COLONOSYM type SEMIOSYM
          ;
identList: IDENTSYM commaIdentifiers { $$ = $2->insert(*$1); }
         | IDENTSYM { $$ = new StrList(*$1); }
         ;
commaIdentifiers: commaIdentifiers commaIdentifier { $$ = $1->add(*$2); }
                | commaIdentifier { $$ = new StrList(*$1); }
                ;
commaIdentifier: COMMAOSYM IDENTSYM { $$ = $2; }
               ;
arrayType:  ARRAYSYM LBRACKETOSYM expression COLONOSYM expression RBRACKETOSYM OFSYM type { $$ = SymbolTable::instance()->arrayType($3, $5, $8); }
         ;
expression: INTOSYM { $$ = SymbolTable::instance()->expression($1); }
          | CHAROSYM { $$ = SymbolTable::instance()->expression_char($1); }
          | STROSYM { $$ = SymbolTable::instance()->expression_string($1); }
          /* IDENTSYM covered by lValue */
          | expression BAROSYM expression { $$ = $1->exec($3, Expression::Bar); }
          | expression AMPOSYM expression { $$ = $1->exec($3, Expression::Amp); }
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
          | TILDEOSYM expression { $$ = $2->exec(Expression::Tilde); }
          | MINUSOSYM expression %prec UMINUSOSYM { $$ = $2->exec(Expression::Negate); }
          | LPARENOSYM expression RPARENOSYM { $$ = $2; }
          | IDENTSYM LPARENOSYM RPARENOSYM { $$ = SymbolTable::instance()->callFunc(*$1); /* function call */ }
          | IDENTSYM LPARENOSYM expressionList RPARENOSYM { $$ = SymbolTable::instance()->callFunc(*$1, $3->list()); /* function call (args) */ }
          | CHRSYM LPARENOSYM expression RPARENOSYM { $$ = $3->exec(Expression::Chr); }
          | ORDSYM LPARENOSYM expression RPARENOSYM { $$ = $3->exec(Expression::Ord); }
          | PREDSYM LPARENOSYM expression RPARENOSYM { $$ = $3->exec(Expression::Pred); }
          | SUCCSYM LPARENOSYM expression RPARENOSYM { $$ = $3->exec(Expression::Succ); }
          | lValue { $$ = $1; }
          ;
expressionList: expression { $$ = new ExprList($1); }
              | expressionList COMMAOSYM expression { $1->add($3); }
              ;
%%

int yyerror(const char* s)
{
  extern char *yytext;// defined and maintained in lex.cpp
  extern int yylineno;// defined and maintained in lex.cpp
  std::cerr << "ERROR! " << s << " " << yytext << " line: " << yylineno << std::endl;
  return 0;
}
