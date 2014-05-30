#include <iostream>
#include <algorithm>

#include "SymbolTable.h"
#include "Type.h"

using namespace std;

extern int yylineno; // defined and maintained in lex.cpp
extern bool bison_verbose;

SymbolTable::SymbolTable()
    : cur_offset(0)
    , ignore_next_lval(false)
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

void SymbolTable::add_symbol(std::string *name, Expression *exp)
{
    exp->symbol->name = *name;

    if(bison_verbose)
        cout << "adding symbol " << *name << " " << exp->toString() << endl;

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

    for(unsigned int i = 0; i < c_symbols.size(); i++)
    {
        if(c_symbols[i]->str_value == *s && c_symbols[i]->type == Type::Const_String)
            return new Expression(c_symbols[i]);
    }

    Symbol *sym = new Symbol(*s);
    c_symbols.push_back(sym);
    return new Expression(sym);
}

Expression* SymbolTable::expression_char(string* s)
{
    if(bison_verbose)
        cout << "expression_char '" << *s << "'" << endl;

    //remove character ticks and add quotes for the expression/symbol name
    auto it = remove_if(std::begin(*s), std::end(*s), [](char c) {return (c == '\'');});
    s->erase(it, std::end(*s));

    *s = "\"" + *s + "\"";

    char c = s->at(1);

    if((*s)[1] == '\\')
    {
        switch((*s)[2])
        {
            case 'n':
                c = '\n';
                break;
            case 'r':
                c = '\r';
                break;
            case 'b':
                c = '\b';
                break;
            case 'f':
                c = '\f';
                break;
            case 't':
                c = '\t';
                break;
            default:
                {
                    if(bison_verbose)
                        cout << "special char used other than regular ones..." << endl;

                    c = (*s)[2];
                }
        }
    }

    for(unsigned int i = 0; i < c_symbols.size(); i++)
    {
        if(c_symbols[i]->char_value == c && c_symbols[i]->type == Type::Const_Char)
            return new Expression(c_symbols[i]);
    }

    Symbol *sym = new Symbol(c, *s);
    c_symbols.push_back(sym);
    return new Expression(sym);
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
    int size = 0;
    for(unsigned int i = 0; i < var_list.size(); i++)
    {
        Symbol *s = new Symbol(var_list[i], cur_offset, Type::fromString(*type_string, false));
        if(bison_verbose)
            cout << "created symbol " << " " << s->toString() << endl;
        symbols.push_back(s);
        if(i > 0)
            names += " ";
        names += var_list[i]; + "[" + to_string(cur_offset) + "]";
        cur_offset += s->size;
        size += s->size;
    }

    names += ")";

    cout << "\t.space " << size << names << " +" << cur_offset << endl;

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
    cout << "main:\n";

    initialize();
}

void SymbolTable::initialize()
{
    Expression *t = new Expression(new Symbol("true", true));
    Expression *T = new Expression(new Symbol("TRUE", true));
    Expression *f = new Expression(new Symbol("false", false));
    Expression *F = new Expression(new Symbol("FALSE", false));

    symbols.push_back(t->symbol);
    symbols.push_back(T->symbol);
    symbols.push_back(f->symbol);
    symbols.push_back(F->symbol);
}

void SymbolTable::end()
{
    cout << "\tli $v0, 10" << endl;
    cout << "\tsyscall" << endl;
    cout << "\t.data" << endl;

    if(bison_verbose)
        cout << "there are " << c_symbols.size() << " constant string entries to add" << endl;

    for(unsigned int i = 0; i < c_symbols.size(); i++)
        c_symbols[i]->store();
}

Expression* SymbolTable::assign(Symbol* s, Expression* e)
{
    if(ignore_next_lval)
        ignore_next_lval = false;
    else
        e->assign(s);

    return e;
}

void SymbolTable::ignoreNextLValue()
{
    ignore_next_lval = true;
}

string* SymbolTable::whileStart()
{
    string *startLbl = new string(Symbol::GetLabel("WS"));
    cout <<  *startLbl << ":" << endl;
    return startLbl;
}

string* SymbolTable::whileExpr(Expression* e)
{
    e->loadInTemp();
    string *endLbl = new string(Symbol::GetLabel("WE"));
    cout << "\tbeq " << e->reg->name() << ", $zero, " << *endLbl << ".stop # exit 'while' branch expr: " << e->toString() << " on line " << yylineno << endl;
    stop_stack.push(*endLbl);

    e->free();

    return endLbl;
}

void SymbolTable::whileStatement(std::string* startLbl, std::string *endLbl)
{
    cout << "\tj " << *startLbl << " # end while on line " << yylineno << endl;
    cout << *endLbl << ".stop:" << endl;
    stop_stack.pop();
}

string* SymbolTable::ifExpr(Expression* e)
{
    string *endLbl = new string(Symbol::GetLabel("IfE"));
    cout << "\tbeq " << e->reg->name() << ", $zero, " << *endLbl << " # exit 'if' branch expr: " << e->toString() << " on line " << yylineno << endl;
    e->free();

    return endLbl;
}

void SymbolTable::ifCondition(std::string* endLbl)
{
    string endifLbl = Symbol::GetLabel("EIf");
    cout << "\tj " << endifLbl << " #end 'if' on line " << yylineno << endl;
    if_stack.push(endifLbl);

    cout << *endLbl << ":" << endl;
}

void SymbolTable::ifStatement()
{
    string lbl = if_stack.top();
    if_stack.pop();
    cout << lbl << ":" << " # end of 'if' statement on line " << yylineno << endl;
}

string* SymbolTable::elseifExpr(Expression *e)
{
    string *lbl = new string(Symbol::GetLabel("IfE"));
    cout << "\tbeq " << e->reg->name() << ", $zero, " << *lbl << " # exit 'elseif' branch expr: " << e->toString() << " on line " << yylineno << endl;
    e->free();
    return lbl;
}

void SymbolTable::elseifStatement(std::string* lbl)
{
    cout << "\tj " << if_stack.top() << endl;
    cout << *lbl << ": " << " # end 'elseif' from line: " << yylineno << endl;
}

void SymbolTable::stop()
{
    string lbl = stop_stack.top();
    cout << "\tj " << lbl << ".stop # stopping on line " << yylineno << endl;
}

string* SymbolTable::forExpr(Expression* lhs, Expression* rhs, Expression::Operation op)
{
    string *lbl = new string(Symbol::GetLabel("For"));
    cout << "\tj " << *lbl << ".begin " << endl;
    cout << *lbl << ".start:" << endl;

    Expression *t = lhs->exec(op);
    t->store();

    cout << *lbl << ".begin:" << endl;
    Expression *lt = rhs->exec(lhs, op == Expression::Succ ? Expression::Gte : Expression::Lte);
    cout << "\tbeq " << lt->reg->name() << ", $zero " << *lbl << ".stop # line " << yylineno << endl;
    stop_stack.push(*lbl);
    lt->free();

    lhs->free();
    rhs->free();
    t->free();

    return lbl;
}

void SymbolTable::forStatement(string* lbl)
{
    std::cout << "\tj " << *lbl << ".start" << endl;
    std::cout << *lbl << ".stop: # end for on line " << yylineno << std::endl;
    stop_stack.pop();
}

std::string* SymbolTable::repeatHead()
{
    string *lbl = new string(Symbol::GetLabel("RS"));
    cout <<  *lbl << ":" << endl;
    stop_stack.push(*lbl);
    return lbl;
}

void SymbolTable::repeatStatement(std::string* lbl, Expression *e)
{
    e->loadInTemp();
    cout << "\tbeq " << e->reg->name() << ", $zero " << *lbl << "# end repeat statemnt on line " << yylineno << endl;
    cout << *lbl << ".stop:" << endl;
    stop_stack.pop();
    e->free();
}
