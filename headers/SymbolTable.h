#ifndef __SYMBOL_TABLE_H__
#define __SYMBOL_TABLE_H__

#include <string>
#include <map>
#include <vector>
#include <stack>

#include "Symbol.h"
#include "Type.h"
#include "Expression.h"

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
        Symbol* findSymbol(std::string*);
        void add_var(std::string *name);
        void add_symbol(std::string *name, Expression* e);
        void add_to_expr_list(Expression* e);
        void add_to_lval_list(Symbol* s);
        void create_vars(std::string *type_string);
        void read();
        void print();
        void begin();
        void initialize();
        void end();
        void checkRegisters();
        Expression* assign(Symbol* s, Expression* e);

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

        std::string* forExpr(Expression* lhs, Expression* rhs, Expression::Operation op);
        void forStatement(std::string* lbl);

        std::string* repeatHead();
        void repeatStatement(std::string* lbl, Expression *e);

        //hackery for now
        Expression* unimp();
        void ignoreNextLValue();

    private:
        std::vector<Expression*> expr_list;
        std::vector<std::string> var_list;
        std::vector<Symbol*> symbols;
        std::vector<Symbol*> c_symbols; //used to write out const string data at end of program
        std::vector<Symbol*> lval_list; //used to store the list of lvals about to be acted on

        std::stack<std::string> if_stack; //labels to keep track of
        std::stack<std::string> stop_stack; //start labels to keep track of in case of stoppage
        int cur_offset;
        bool ignore_next_lval;
};

#endif //__SYMBOL_TABLE_H__
