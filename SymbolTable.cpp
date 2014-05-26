#include <iostream>
#include <algorithm>

#include "SymbolTable.h"
#include "Type.h"

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

void SymbolTable::add_const(std::string *name, Expression *exp)
{
    exp->symbol->name = *name;

    if(bison_verbose)
        cout << "adding const symbol " << *name << " " << exp->toString() << endl;

    symbols.push_back(exp->symbol);
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
    return new Expression(new Symbol(i));
}

Expression* SymbolTable::unimp()
{
    return new Expression(new Symbol(1)); //TODO: this is so we don't get errors.
    //cerr << "Unimplemented function " << __FUNCTION__ << " line: " << yylineno << endl;
    //exit(1);
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

    //TODO: find similar string names so there are no duplicates in data section
    Expression *e = new Expression(new Symbol(*s));
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
    Expression *e = new Expression(new Symbol(*s, Type::Const_Char));
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
                cout << "found symbol " << *s << " " << symbols[i] << endl;
            return symbols[i];
        }
    }

    cerr << "could not find symbol " << *s << " line: " << yylineno << endl;
    exit(1);
}

void SymbolTable::create_vars(std::string *type_string)
{
    string names = " #(";
    for(unsigned int i = 0; i < var_list.size(); i++)
    {
        Symbol *s = new Symbol(var_list[i], cur_offset, Type::fromString(*type_string));
        if(bison_verbose)
            cout << "created symbol " << " " << s->toString() << endl;
        symbols.push_back(s);
        if(i > 0)
            names += " ";
        names += var_list[i];// + "[" + cur_offset + "]";
        cur_offset += 4; //TODO: fix this for types to have size
    }

    names += ")";

    //TODO: int->str
    cout << "\t.space " << var_list.size() * 4 << names << " +" << cur_offset << endl;

    var_list.clear();
}

//TODO: make this generic with a type instead of always integers
//also include which scope to create the variable
void SymbolTable::create_symbol(string name, int value)
{
    //TODO: fix this
    Symbol *s = new Symbol(name, cur_offset, Type::Integer);
    if(bison_verbose)
        cout << "creating symbol " << name << " " << s << endl;

    symbols.push_back(s);
    cur_offset += 4;
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
        cerr << "failed to release all registers! line: " << yylineno << endl;
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

    initialize();
}

void SymbolTable::initialize()
{
    //create_symbol("true", 1);
    //create_symbol("false", 0);
}

void SymbolTable::end()
{
    cout << "\tli $v0, 10" << endl;
    cout << "\tsyscall" << endl;
    cout << "\t.data" << endl;

    if(bison_verbose)
        cout << "there are " << str_expr_list.size() << " constant string entries to add" << endl;

    for(unsigned int i = 0; i < str_expr_list.size(); i++)
        str_expr_list[i]->store();
}

void SymbolTable::assign(Symbol* s, Expression* e)
{
    e->assign(s);

}
