#include <string>
#include <iostream>
#include <map>

#include "Symbol.h"
#include "SymbolTable.h"
#include "Register.h"
#include "Type.h"
#include "Log.h"

using namespace std;

extern bool bison_verbose;
extern int yylineno;
extern Log* cpsl_log;

Symbol::Symbol(Symbol* s)
{
    name = s->name;
    str_value = s->str_value;
    int_value = s->int_value;
    bool_value = s->bool_value;
    char_value = s->char_value;
    offset = s->offset;
    type = s->type;
    subType = s->subType;
    regPointer = s->regPointer;
    rp = s->rp;
    s->rp = NULL;
}

Symbol::Symbol(std::string& name, int offset, Type *type, bool global)
    : name(name)
    , offset(offset)
    , type(type)
    , regPointer( global ? "$gp" : "$fp" )
    , rp(NULL)
{
    if(!type)
    {
        std::cerr << "attempt to create symbol with NULL type on line " << yylineno << std::endl;
        exit(1);
    }
}

Symbol::Symbol(std::string& name, int offset, Type *type, Register* _reg)
    : name(name)
    , offset(offset)
    , type(type)
    , regPointer(_reg->name())
    , rp(_reg)
{
    if(!type)
    {
        std::cerr << "attempt to create symbol with NULL type on line " << yylineno << std::endl;
        exit(1);
    }
}

std::string Symbol::GetLabel(const string& prefix, bool gen)
{
    static map<string, int> m;
    if(!m.count(prefix))
        m[prefix] = 0;
    if(gen)
        return prefix + to_string(m[prefix]++);
    else
        return prefix + to_string(m[prefix]-1);
}

void Symbol::read()
{
    if(bison_verbose)
        cout << "\treading symbol " << toString() << endl;

    if(type->isConst())
    {
        if(bison_verbose) //skip this error (for files types we don't support yet)
        {
            cerr << "attempting to read a value into a const var " << toString() << " on line " << yylineno << endl;
            exit(1);
        }
        return;
    }

    switch(type->vt)
    {
        case Type::Integer:
            {
                cpsl_log->out << "\tli $v0, 5 #read " << toString() << " on line: " << yylineno << endl;
                cpsl_log->out << "\tsyscall" << endl;

                cpsl_log->out << "\tsw $v0, " << offset << "(" << reg() << ")" << endl;
            }
            break;
        case Type::Char:
            {
                cpsl_log->out << "\tli $v0, 12 #read " << toString() << " on line: " << yylineno << endl;
                cpsl_log->out << "\tsyscall" << endl;

                cpsl_log->out << "\tsb $v0, " << offset << "(" << reg() << ")" << endl;
            }
            break;
        case Type::Bool:
            {
                cpsl_log->out << "\tli $v0, 5 #read " << toString() << " on line: " << yylineno << endl;
                cpsl_log->out << "\tsyscall" << endl;

                cpsl_log->out << "\tsb $v0, " << offset << "(" << reg() << ")" << endl;
            }
            break;
        case Type::Unknown:
            {
                //Do nothing for unknown types right now
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
    string o = Type::toString(type->vt);
    o += " " + name;

    switch(type->vt)
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
            o += " val=" + to_string(bool_value);
            break;
        default:
            break;
    }

    return o;
}

void Symbol::store()
{
    if(bison_verbose)
        cout << "storing " << toString() << endl;

    switch(type->vt)
    {
        case Type::Const_String:
        case Type::Const_Char:
            cpsl_log->out << name << ":\t.asciiz " << str_value << endl;
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
    switch(type->vt)
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
            type = SymbolTable::instance()->typeOf(vt);
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

    type = SymbolTable::instance()->typeOf(vt);
}

std::string Symbol::reg()
{
    return regPointer;
}
