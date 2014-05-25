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

Expression* Expression::add(Expression* e)
{
    if(canFold(e))
    {
        if(bison_verbose)
            cout << "const adding " << const_int << "+" << e->const_int << endl;
        const_int += e->const_int;
        delete e;
    }
    else if(type == SYM || e->type == SYM)
    {
        loadInTemp();
        e->loadInTemp();

        cout << "\tadd " << reg->name() << ", " << reg->name() << ", " << e->reg->name() << endl;

        e->free();

        Expression *e = new Expression(reg);
        reg = NULL; //hand off register to new expression

        return e;
    }
    else
    {
        cerr << "Unimplemented or incorret add type " << type << " " << e->type << " line: " << yylineno << endl;
        exit(1);
    }

    return this;
}

string Expression::toString(Operation op)
{
    switch(op)
    {
        case Add:
              return "add";
        case Sub:
              return "subtract";
        case Mul:
              return "multiply";
        case Div:
              return "divide";
        case Mod:
              return "mod";
        default:
              cerr << "Unknown op " << op << " in function " << __FUNCTION__ << " line: " << yylineno << endl;
              exit(1);
    }
}

Expression* Expression::exec(Expression* e, Operation op)
{
    if(canFold(e))
    {
        if(bison_verbose)
            cout << "const " << toString(op) << " folding " << const_int << ", " << e->const_int << endl;

        switch(op)
        {
            case Add:
                const_int += e->const_int;
                break;
            case Sub:
                const_int -= e->const_int;
                break;
            case Mul:
                const_int *= e->const_int;
                break;
            case Div:
                const_int /= e->const_int;
                break;
            case Mod:
                const_int = const_int % e->const_int;
                break;
        }
        delete e;
    }
    else if(type == SYM || e->type == SYM)
    {
        loadInTemp();

        string rhs;
        if(e->type == SYM)
        {
            e->loadInTemp();
            rhs = e->reg->name();
        }
        else
        {
            rhs = to_string(e->const_int);
        }

        switch(op)
        {
            case Add:
                cout << "\tadd " << reg->name() << ", " << reg->name() << ", " << rhs << endl;
                break;
            case Sub:
                cout << "\tsub " << reg->name() << ", " << reg->name() << ", " << rhs << endl;
                break;
            case Mul:
                cout << "\tmul " << reg->name() << ", " << reg->name() << ", " << rhs << endl;
                break;
            case Div:
                cout << "\tdiv " << reg->name() << ", " << reg->name() << ", " << rhs << endl;
                break;
            case Mod:
                cout << "\tdiv " << reg->name() << ", " << reg->name() << ", " << rhs << endl;
                cout << "\tmfhi " << reg->name() << " # get quotient of div" << endl;
                break;
        }

        if(e->reg)
            e->free();

        Expression *e = new Expression(reg);
        reg = NULL; //hand off register to new expression

        return e;
    }
    else
    {
        cerr << "Unimplemented or incorret add type " << type << " " << e->type << " line: " << yylineno << endl;
        exit(1);
    }

    return this;
}

bool Expression::canFold(Expression* e)
{
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
        default:
            break;
    }

    if(symbol)
        o += " " + symbol->name + symbol->str_val();

    if(reg)
        o += " register " + reg->name();

    return o;
}

int Expression::getInt()
{
    if(type == SYM)
    {
        cerr << "getInt() missing symbol on expression " << " line: " << yylineno << endl;
        exit(1);
    }

    return type == INT ? const_int : symbol->value;
}

void Expression::print()
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
            {
                cout << "\tli $v0 1" << endl;
                if(reg)
                {
                    cout << "\tadd $a0, " << reg->name() << ", 0" << endl;
                    free();
                }
                else if(symbol)
                {
                    cout << "\tlw $a0, " << symbol->offset << "($gp)" << endl;
                }
                else
                {
                    cerr << "No symbol or register for expression " << toString() << " line: " << yylineno << endl;
                    cout << symbol->name << endl;
                    exit(1);
                }
            }
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

void Expression::loadInTemp()
{
    if(reg)//already done!
        return;

    reg = Register::FindRegister(Register::Temp);

    if(type == SYM)
    {
        cout << "\tlw " << reg->name() << ", " << symbol->offset << "($gp)" << endl;
    }
    else if(type == INT)
    {
        cout << "\tli " << reg->name() << ", " << const_int << endl;
    }
    else
    {
        cerr << "Attempting to load string constant into register invalid or unimplemented on line: " << yylineno << endl;
        exit(1);
    }
}

void Expression::free()
{
    if(reg)
    {
        Register::ReleaseRegister(reg);
        reg = NULL;
    }
}
