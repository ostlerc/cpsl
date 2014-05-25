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

    return "unknown";
}

string Expression::toString()
{
    string o;

    if(type != SYM)
        o = "const ";

    o += typeName();

    switch(type)
    {
        case INT:
            o += " " + to_string(const_int);
            break;
    }

    if(symbol)
        o += " " + symbol->name + symbol->str_val();

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

void Expression::write()
{
    if(bison_verbose)
        cout << "writing expression " << toString() << endl;

    switch(type)
    {
        case INT:
            {
                cout << "\tli $v0, 1" << endl;
                cout << "\tli $a0, " << getInt() << " #" << toString() << " line: " << yylineno << endl;
            }
            break;
        case STR:
            {
                cout << "\tli $v0, 4" << endl;
                cout << "\tla $a0, " << symbol->name << " # " << symbol->str_value << " line: " << yylineno << endl;
            }
            break;
        case SYM:
            if(!reg)
                reg = Register::FindRegister(Register::Temp);

            cout << "\tli $v0 1" << endl;
            cout << "\tla " << reg->name() << " dat" << endl;
            cout << "\tlw $a0, " << symbol->offset << "(" << reg->name() << ")" << endl;
            reg = NULL;
            //cout << "\tlw " << reg->name() << ", " << "0"<< "($fp) #" << toString() << endl;
            break;
    }

    cout << "\tsyscall" << endl;
}

void Expression::store()
{
    if(bison_verbose)
        cout << "storing " << toString() << endl;

    if(type != STR)
    {
        cerr << "Error trying to store non const string on line: " << yylineno << endl;
        exit(1);
    }

    if(!symbol)
    {
        cerr << "Error trying to store const string with NULL symbol on line: " << yylineno << endl;
        exit(1);
    }

    cout << symbol->name << ":\t.asciiz" << symbol->str_value << endl;
}
