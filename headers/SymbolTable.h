#ifndef __SYMBOL_TABLE_H__
#define __SYMBOL_TABLE_H__

#include <string>
#include <map>
#include <vector>

#include "Symbol.h"
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
        void add_to_expr_list(Expression* e);
        void add_to_lval_list(Symbol* s);
        void create_vars(int type);
        void read();
        void write();
        void begin();
        void end();

    private:
        std::vector<Expression*> expr_list;
        std::vector<std::string> var_list;
        std::vector<Symbol*> symbols;
        std::vector<Expression*> str_expr_list; //used to write out const string data at end of program
        std::vector<Symbol*> lval_list; //used to store the list of lvals about to be acted on
        int cur_offset;
};

#endif //__SYMBOL_TABLE_H__
