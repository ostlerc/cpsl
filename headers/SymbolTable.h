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
        Expression* lValue(Symbol* s);
        Symbol* findSymbol(std::string*);
        void add_var(std::string *name);
        void create_vars(int type);

    private:
        std::vector<std::string> var_list;
        std::vector<Symbol*> symbols;
        int cur_offset;
};

#endif //__SYMBOL_TABLE_H__
