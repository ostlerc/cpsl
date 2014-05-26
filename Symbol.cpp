#include <string>
#include <iostream>

#include "Symbol.h"
#include "SymbolTable.h"
#include "Register.h"
#include "Type.h"

using namespace std;

extern bool bison_verbose;
extern int yylineno;

Symbol::Symbol(std::string& str_value, Type::ValueType vt, int offset)
    : str_value(str_value)
    , offset(offset)
    , type(vt)
{
    if(offset != -1 && Type::isConst(type))
    {
        cerr << "attempt to create symbol with offset with const type on line: " << yylineno << endl;
        exit(1);
    }

    string prefix = Type::prefix(type);
    name = NewLabel(prefix);
}

Symbol::Symbol(Symbol* s)
{
    name = s->name;
    str_value = s->str_value;
    int_value = s->int_value;
    bool_value = s->bool_value;
    char_value = s->char_value;
    offset = s->offset;
    type = s->type;
    size = s->size;
}

std::string Symbol::NewLabel(const string& prefix)
{
    static int at = 0;
    return prefix + to_string(at++);
}

void Symbol::read()
{
    if(bison_verbose)
        cout << "\treading symbol " << toString() << endl;

    if(Type::isConst(type))
    {
        if(bison_verbose) //skip this error (for files types we don't support yet)
        {
            cerr << "attempting to read a value into a const var " << toString() << " on line " << yylineno << endl;
            exit(1);
        }
        return;
    }

    switch(type)
    {
        case Type::Integer: 
            {
                cout << "\tli $v0, 5 #read " << toString() << " on line: " << yylineno << endl;
                cout << "\tsyscall" << endl;

                cout << "\tsw $v0, " << offset << "($gp)" << endl;
            }
            break;
        case Type::Char:
            {
                cout << "\tli $v0, 12 #read " << toString() << " on line: " << yylineno << endl;
                cout << "\tsyscall" << endl;

                cout << "\tsb $v0, " << offset << "($gp)" << endl;
            }
            break;
        case Type::Bool:
            {
                cout << "\tli $v0, 5 #read " << toString() << " on line: " << yylineno << endl;
                cout << "\tsyscall" << endl;

                cout << "\tsb $v0, " << offset << "($gp)" << endl;
            }
            break;
        default:
            cerr << "Unsupported read type " << toString() << " on line " << yylineno << endl;
            exit(1);
            break;
    }
}

string Symbol::toString()
{
    string o = Type::toString(type);
    o += " " + name;

    switch(type)
    {
        case Type::Const_Integer:
            o += " val=" + to_string(int_value);
            break;
        case Type::Const_String:
            o += " val=" + str_value;
            break;
        case Type::Const_Char:
            o += string(" val='") + char_value + string("'");
            break;
        case Type::Const_Bool:
            o += " val=" + bool_value;
            break;
        default:
            break;
    }

    return o;
    //TODO: show const symbols values here
}

void Symbol::store()
{
    if(bison_verbose)
        cout << "storing " << toString() << endl;

    switch(type)
    {
        case Type::Const_String:
        case Type::Const_Char:
            cout << name << ":\t.asciiz " << str_value << endl;
            break;
        default:
            {
                cerr << "attempt to store symbol incorrect or unimplemented type (" << toString() << ") on line: " << yylineno << endl;
                exit(1);
            }
            break;
    }
}

void Symbol::setType(Type::ValueType vt)
{
    int v = -1;
    switch(type)
    {
        case Type::Const_Integer:
            v = int_value;
            break;
        case Type::Const_Char:
            v = char_value;
            break;
        case Type::Const_Bool:
            v = bool_value;
            break;
        default:
            type = vt;
            return;
            break;
    }

    switch(vt)
    {
        case Type::Const_Integer:
        case Type::Integer:
            int_value = v;
            break;
        case Type::Const_Char:
        case Type::Char:
            char_value = v;
            break;
        case Type::Const_Bool:
        case Type::Bool:
            bool_value = v;
            break;
        default:
            break;
    }

    type = vt;
}

