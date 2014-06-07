#ifndef __SYMBOL_TABLE_H__
#define __SYMBOL_TABLE_H__

#include <string>
#include <map>
#include <vector>
#include <stack>

#include "Symbol.h"
#include "Type.h"
#include "Expression.h"

class SymbolTableLevel;

typedef ListWrapper<std::string> StrList;

struct Parameters
{
    Parameters(StrList* sl, Type::ValueType type) : vars(sl) , type(type) {}
    StrList* vars;
    Type::ValueType type;
};

typedef ListWrapper<Parameters> ParamList;

class SymbolTable
{
    //Private constructor
    SymbolTable();
    virtual ~SymbolTable();
    public:
        static SymbolTable* instance();

        Expression* expression(int);
        Expression* expression(std::string*);
        Expression* expression_string(std::string*);
        Expression* expression_char(std::string*);
        Expression* lValue(Symbol* s);
        Symbol* findSymbol(std::string*, bool err = true);
        Symbol* findSymbol(std::string, bool err = true);
        void declare_const(std::string *name, Expression* e);
        void add_to_expr_list(Expression* e);
        void add_to_lval_list(Symbol* s);
        void create_vars(std::string *type_string, StrList* var_list);
        void read();
        void print(ExprList* expr_list);
        void begin();
        void initialize();
        void end();
        void checkRegisters();
        void enterScope();
        void exitScope();
        Expression* assign(Symbol* s, Expression* e);

        void push(std::string reg);
        void pop(std::string reg);
        void set(std::string lhs, std::string rhs);

        //while
        std::string* whileStart();
        std::string* whileExpr(Expression* e);
        void whileStatement(std::string* startLbl, std::string *endLbl);
        void stop();

        //if
        std::string* ifExpr(Expression* e);
        void ifCondition(std::string* endLbl);
        void ifStatement();
        std::string* elseifExpr(Expression *e);
        void elseifStatement(std::string* lbl);

        //for
        std::string* forExpr(Expression* lhs, Expression* rhs, Expression::Operation op);
        void forStatement(std::string* lbl);

        //repeat
        std::string* repeatHead();
        void repeatStatement(std::string* lbl, Expression *e);

        //hackery for now
        Expression* unimp();
        void ignoreNextLValue();

        Symbol* forwardProc(std::string* id, ParamList* params);
        void procedureHead();
        void procedureParams(std::string* id, ParamList* params);
        void endProcedure();
        void callProc(std::string* proc, ExprList* expr_list = NULL);

        std::string paramsString(Parameters params);
        std::string paramsString(std::vector<Expression*>& exprs);
        std::string procId(std::string id, Parameters params);
        std::string procId(std::string id, std::vector<Parameters>& params);
        std::string procId(std::string id, ExprList* exprs);

    private:

        std::vector<Symbol*> c_symbols; //used to write out const string data at end of program, there is no scope for const literals
        std::vector<Symbol*> lval_list; //used to store the list of lvals about to be acted on
        std::vector<SymbolTableLevel*> levels;

        std::stack<std::string> if_stack; //labels to keep track of
        std::stack<std::string> stop_stack; //start labels to keep track of in case of stoppage
        bool ignore_next_lval; //TODO: remove when types are done
};


#endif //__SYMBOL_TABLE_H__
