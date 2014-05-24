#include <iostream>
#include <algorithm>

#include "SymbolTable.h"

using namespace std;

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
        cout << "already have defined variable " << *name << endl;
        exit(1);
    }
    cout << "added var " << *name << endl;
    var_list.push_back(*name);
}

Expression* SymbolTable::expression (int i)
{
    cout << "expression int " << i << endl;
    return new Expression(i);
}

Expression* SymbolTable::expression(string* s)
{
    cout << "expression string '" << s->c_str() << "'" << endl;
    return new Expression(findSymbol(s));
}

Expression* SymbolTable::lValue(Symbol* s)
{
    return new Expression(s);
}

Expression* SymbolTable::expression_string(string* s)
{
    cout << "expression string '" << s->c_str() << "'" << endl;
    return new Expression(*s);
}

Symbol* SymbolTable::findSymbol(string* s)
{
    for(unsigned int i = 0; i < symbols.size(); i++)
    {
        if(symbols[i]->name == *s)
        {
            cout << "found symbol " << *s << " val: " << symbols[i]->value << endl;
            return symbols[i];
        }
    }

    cerr << "could not find symbol " << *s << endl;
    exit(1);
}

void SymbolTable::create_vars(int type)
{
    if(type != 0) //0 is int for now. in the future a type system will be added
    {
        cerr << "undefined type " << type << endl;
        exit(1);
    }

    for(unsigned int i = 0; i < var_list.size(); i++)
    {
        symbols.push_back(new Symbol(var_list[i], 0, cur_offset));
        cur_offset -= 4;
    }

    var_list.clear();
}
