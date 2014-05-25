#include <iostream>
#include <algorithm>

#include "SymbolTable.h"

using namespace std;

extern int yylineno; // defined and maintained in lex.cpp
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
        cout << "expression(string) '" << *s << "'" << endl;
    Symbol *sym = findSymbol(s);

    Expression *e = new Expression(sym);
    return e;
}

Expression* SymbolTable::lValue(Symbol* s)
{
    Expression *e = new Expression(s);
    return e;
}

Expression* SymbolTable::expression_string(string* s)
{
    if(bison_verbose)
        cout << "expression_string '" << *s << "'" << endl;
    Expression *e = new Expression(*s);
    str_expr_list.push_back(e);
    return e;
}

Expression* SymbolTable::expression_char(string* s)
{
    if(bison_verbose)
        cout << "expression_char '" << *s << "'" << endl;

    //remove character ticks and add quotes for the expression/symbol name
    auto it = remove_if(std::begin(*s), std::end(*s), [](char c) {return (c == '\'');});
    s->erase(it, std::end(*s));

    *s = "\"" + *s + "\"";

    //TODO: find similar string names so there are no duplicates in data section
    Expression *e = new Expression(*s);
    str_expr_list.push_back(e);
    return e;
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

    string names = " #(";
    for(unsigned int i = 0; i < var_list.size(); i++)
    {
        Symbol *s = new Symbol(var_list[i], 1, cur_offset);
        if(bison_verbose)
            cout << "creating symbol " << var_list[i] << " " << s << endl;
        symbols.push_back(s);
        if(i > 0)
            names += " ";
        names += var_list[i];// + "[" + cur_offset + "]";
        cur_offset += 4;
    }

    names += ")";

    //TODO: int->str
    cout << "\t.space " << var_list.size() * 4 << names << " +" << cur_offset << endl;

    var_list.clear();
}

void SymbolTable::add_to_expr_list(Expression* e)
{
    if(bison_verbose)
        cout << "added expr " << e->toString() << " line: " << yylineno << endl;
    expr_list.push_back(e);
}

void SymbolTable::print()
{
    if(expr_list.size() == 0)
    {
        cerr << "Missing arguments on line: " << yylineno << endl;
        exit(1);
    }

    for(unsigned int i = 0; i < expr_list.size(); i++)
        expr_list[i]->print();

    checkRegisters();

    expr_list.clear();
}

void SymbolTable::checkRegisters()
{
    if(Register::reservedRegisters() > 0)
    {
        cerr << "failed to release all registers!" << endl;
        exit(1);
    }
}

void SymbolTable::add_to_lval_list(Symbol* s)
{
    if(bison_verbose)
        cout << "added lval (from list)" << s->name << " line: " << yylineno << endl;

    lval_list.push_back(s);
}

void SymbolTable::read()
{
    if(lval_list.size() == 0)
    {
        cerr << "Missing arguments on line: " << yylineno << endl;
        exit(1);
    }

    for(unsigned int i = 0; i < lval_list.size(); i++)
        lval_list[i]->read();

    lval_list.clear();
}

void SymbolTable::begin()
{
    cout << "\t.text" << endl;
    cout << "main:";
}

void SymbolTable::end()
{
    cout << "\tli $v0, 10" << endl;
    cout << "\tsyscall" << endl;
    cout << "\t.data" << endl;

    if(bison_verbose)
        cout << "there are " << str_expr_list.size() << " constant string entries to add" << endl;

    for(int i = 0; i < str_expr_list.size(); i++)
        str_expr_list[i]->store();
}
