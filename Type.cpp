#include "Type.h"

#include <iostream>

using namespace std;

extern int yylineno;
extern int bison_verbose;

Type::Type(ValueType type, int size) 
    : vt(type) 
    , size(size)
{
}
Type::~Type() {}

bool Type::isConst(ValueType vt)
{
    switch(vt)
    {
        case Const_Integer:
        case Const_String:
        case Const_Char:
        case Const_Bool:
            return true;
        default:
            return false;
    }
}

string Type::toString(ValueType vt)
{
    switch(vt)
    {
        case Const_Integer:
            return "const_integer";
        case Const_String:
            return "const_string";
        case Const_Char:
            return "const_char";
        case Const_Bool:
            return "const_boolean";
        case Integer:
            return "integer";
        case Char:
            return "char";
        case Bool:
            return "boolean";
        case Unknown:
            return "unknown";
        case Procedure:
            return "procedure";
        case Function:
            return "function";
        case Record:
            return "record";
        case Array:
            return "array";
        default:
            cerr << "Unknown valuetype " << vt << " on line: " << yylineno << endl;
            exit(1);
    }
}

string Type::prefix(ValueType vt)
{
    switch(vt)
    {
        case Const_Integer:
            return "cint";
        case Const_String:
            return "cstr";
        case Const_Char:
            return "cchr";
        case Const_Bool:
            return "cbol";
        case Integer:
            return "int";
        case Char:
            return "char";
        case Bool:
            return "bool";
        case Record:
            return "record";
        case Array:
            return "array";
        default:
            cerr << "Unknown valuetype " << vt << " on line: " << yylineno << endl;
            exit(1);
    }
}

bool Type::isFoldable(ValueType type)
{
    switch(type)
    {
        case Const_Integer:
        case Const_Bool:
            return true;
        case Const_String:
        case Const_Char:
        case Integer:
        case Char:
        case Bool:
        case Unknown:
        case Record:
        case Array:
            return false;
        default:
            cerr << "Unknown valuetype " << toString(type) << " on line: " << yylineno << endl;
            exit(1);
    }
}

bool Type::match(ValueType lhs, ValueType rhs)
{
    if(lhs == rhs)
        return true;

    if(lhs == Const_String || rhs == Const_String)
        return false;

    if(lhs == Function || rhs == Function)
        return false;

    if(lhs == Procedure || rhs == Procedure)
        return false;

    if(lhs == Unknown || rhs == Unknown)
        return false;

    //if(nonconst_val(lhs) == nonconst_val(rhs))
    return true;
}

auto Type::nonconst_val(ValueType type) -> ValueType
{
    if(!isConst(type))
        return type;

    switch(type)
    {
        case Const_Integer:
            return Integer;
        case Const_Char:
            return Char;
        case Const_Bool:
            return Bool;
        case Const_String:
            {
                cerr << "cannot have non const value of string. line:" << yylineno << endl;
                exit(1);
            }
            break;
        default:
            {
                cerr << "Unknown const type found in nonconst_val on line: " << yylineno << endl;
                exit(1);
            }
            break;
    }
}

auto Type::const_val(ValueType type) -> ValueType
{
    if(isConst(type))
        return type;

    switch(type)
    {
        case Integer:
            return Const_Integer;
        case Char:
            return Const_Char;
        case Bool:
            return Const_Bool;
        default:
            {
                cerr << "Unknown non_const type found in const_val on line: " << yylineno << endl;
                exit(1);
            }
            break;
    }
}
