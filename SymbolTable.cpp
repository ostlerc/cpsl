#include <iostream>
#include <algorithm>

#include "SymbolTable.h"

using namespace std;

extern int yylineno;// defined and maintained in lex.cpp
extern bool bison_verbose;

SymbolTable::SymbolTable()
    : cur_offset(0)
{ }

SymbolTable::~SymbolTable()
{ }

SymbolTable* SymbolTable::instance()
{
    static SymbolTable* st = NULL;
    if(!st)
        st = new SymbolTable;

    return st;
}

void SymbolTable::add_var(std::string *name)
{
    auto it = find(var_list.begin(), var_list.end(), *name);
    if(it != var_list.end())
    {
        cerr << "already have defined variable " << *name << " line: " << yylineno << endl;
        exit(1);
    }
    if(bison_verbose)
        cout << "added var " << *name << endl;
    var_list.push_back(*name);
}

Expression* SymbolTable::expression (int i)
{
    if(bison_verbose)
        cout << "expression int " << i << endl;
    return new Expression(i);
}

Expression* SymbolTable::expression(string* s)
{
    if(bison_verbose)
        cout << "expression(string) '" << s->c_str() << "'" << endl;
    Symbol *sym = findSymbol(s);
    return new Expression(sym);
}

Expression* SymbolTable::lValue(Symbol* s)
{
    Expression *e = new Expression(s);
    return e;
}

Expression* SymbolTable::expression_string(string* s)
{
    if(bison_verbose)
        cout << "expression_string '" << s->c_str() << "'" << endl;
    return new Expression(*s);
}

Symbol* SymbolTable::findSymbol(string* s)
{
    for(unsigned int i = 0; i < symbols.size(); i++)
    {
        if(symbols[i]->name == *s)
        {
            if(bison_verbose)
                cout << "found symbol " << *s << " val: " << symbols[i]->value << " " << symbols[i] << endl;
            return symbols[i];
        }
    }

    cerr << "could not find symbol " << *s << " line: " << yylineno << endl;
    exit(1);
}

void SymbolTable::create_vars(int type)
{
    if(type != 0) //0 is int for now. in the future a type system will be added
    {
        cerr << "undefined type " << type << " line: " << yylineno << endl;
        exit(1);
    }

    for(unsigned int i = 0; i < var_list.size(); i++)
    {
        Symbol *s = new Symbol(var_list[i], 1, cur_offset);
        if(bison_verbose)
            cout << "creating symbol " << var_list[i] << " " << s << endl;
        cout << "\t.data" << var_list[i] << "\t" << endl;
        symbols.push_back(s);
        cur_offset -= 4;
    }

    var_list.clear();
}
