#include "Type.h"

#include <iostream>

using namespace std;

extern int yylineno;
extern int bison_verbose;

Type::Type(std::string name, ValueType type, int size, bool _const, Type* nonc) 
    : name(name)
    , vt(type) 
    , size(size)
    , _const(_const)
    , nonconst_counterpart(nonc)
    , start_index(0)
    , array_type(NULL)
{ }

Type::~Type() {}

bool Type::isConst()
{
    return _const;
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

Type* Type::nonconst_val()
{
    if(!_const)
        return this;

    if(!nonconst_counterpart)
    {
        cerr << "attempting to get nonconst_counterpart on line " << yylineno << endl;
        exit(1);
    }

    return nonconst_counterpart;
}

auto Type::const_val(ValueType type) -> ValueType
{
    switch(type)
    {
        case Const_String:
            return Const_String;
        case Integer:
        case Const_Integer:
            return Const_Integer;
        case Char:
        case Const_Char:
            return Const_Char;
        case Bool:
        case Const_Bool:
            return Const_Bool;
        default:
            {
                cerr << "Unknown non_const type found in const_val on line: " << yylineno << endl;
                exit(1);
            }
            break;
    }
}

std::string Type::toString() const
{
    if(vt == Function || vt == Procedure || vt == Array || vt == Record)
        return toString(vt) + " " + name;

    return toString(vt);
}
