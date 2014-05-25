#include "Expression.h"

#include <iostream>

using namespace std;

extern int yylineno;// defined and maintained in lex.cpp
extern bool bison_verbose;

Expression* Expression::unimp(Expression* e)
{
    cerr << "Unimplemented function " << __FUNCTION__ << " line: " << yylineno << endl;
    exit(1);
}

Expression* Expression::plus(Expression* e)
{
    if(canFold(e))
    {
        if(bison_verbose)
            cout << "const adding " << const_int << "+" << e->const_int << endl;
        const_int += e->const_int;
        delete e;
    }
    else if(type == SYM && e->type == SYM)
    {
        return new Expression(getInt() * e->getInt());
    }
    else
    {
        cerr << "Unimplemented or incorret add type " << type << " " << e->type << " line: " << yylineno << endl;
        exit(1);
    }

    return this;
}

Expression* Expression::sub(Expression* e)
{
    if(bison_verbose)
        cout << "sub " << toString() << " " << e->toString() << endl;
    if(canFold(e))
    {
        if(bison_verbose)
            cout << "const subtracting " << const_int << "-" << e->const_int << endl;
        const_int -= e->const_int;
        delete e;
    }
    else if(type == SYM && e->type == SYM)
    {
        return new Expression(getInt() - e->getInt());
    }
    else
    {
        cerr << "unimp sub with non const" << " line: " << yylineno << endl;
        exit(1);
    }

    return this;
}

Expression* Expression::mul(Expression* e)
{
    if(canFold(e))
    {
        if(bison_verbose)
            cout << "const multiplying " << const_int << "*" << e->const_int << endl;
        const_int *= e->const_int;
        delete e;
    }
    else if(type == SYM && e->type == SYM)
    {
        return new Expression(getInt() * e->getInt());
    }
    else
    {
        cerr << "unimp mul with non const" << " line: " << yylineno << endl;
        exit(1);
    }

    return this;
}


Expression* Expression::div(Expression* e)
{
    if(canFold(e))
    {
        if(bison_verbose)
            cout << "const dividing " << const_int << "/" << e->const_int << endl;
        const_int /= e->const_int;
        delete e;
    }
    else if(type == SYM && e->type == SYM)
    {
        int t = e->getInt();
        if(t == 0)
        {
            cerr << "error div by zero on line " << yylineno << endl;
            exit(1);
        }

        return new Expression(getInt() / e->getInt());
    }
    else
    {
        cerr << "unimp div with non const" << " line: " << yylineno << endl;
        exit(1);
    }

    return this;
}

Expression* Expression::mod(Expression* e)
{
    if(canFold(e))
    {
        if(bison_verbose)
            cout << "const mod'ing " << const_int << "%" << e->const_int << endl;
        const_int = const_int % e->const_int;
        delete e;
    }
    else if(type == SYM && e->type == SYM)
    {
        return new Expression(getInt() % e->getInt());
    }
    else
    {
        cerr << "unimp mod with non const" << " line: " << yylineno << endl;
        exit(1);
    }

    return this;
}

bool Expression::canFold(Expression* e)
{
    if(type != e->type)
    {
        cerr << "type mismatch " << toString() << " " << e->toString() << " line: " << yylineno << endl;
    }
    return type == INT && e->type == INT;
}

string Expression::typeName()
{
    switch(type)
    {
        case INT:
            return "integer";
            break;
        case STR:
            return "string";
            break;
        case SYM:
            return "symbol";
            break;
    }
}

string Expression::toString()
{
    string o;

    if(type != SYM)
        o = "const ";

    o += typeName();

    if(symbol)
        o += " " + symbol->name;

    return o;
}

int Expression::getInt()
{
    if(type == SYM && !symbol)
    {
        cerr << "getInt() missing symbol on expression " << " line: " << yylineno << endl;
        exit(1);
    }

    return type == INT ? const_int : symbol->value;
}
