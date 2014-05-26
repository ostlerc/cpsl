#include "Type.h"

#include <iostream>

using namespace std;

extern int yylineno;
extern int bison_verbose;

Type::Type(ValueType type) : vt(type) {}
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
            return "const integer";
        case Const_String:
            return "const string";
        case Const_Char:
            return "const char";
        case Const_Bool:
            return "const bool";
        case Integer:
            return "integer";
        case Char:
            return "char";
        case Bool:
            return "bool";
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
        default:
            cerr << "Unknown valuetype " << vt << " on line: " << yylineno << endl;
            exit(1);
    }
}

Type::ValueType Type::fromString(std::string type_string, bool is_const)
{
    if(type_string == "integer" || type_string == "INTEGER")
        return is_const ? Const_Integer : Integer;
    else if(type_string == "boolean" || type_string == "BOOLEAN")
        return is_const ? Const_Bool : Bool;
    else if(type_string == "char" || type_string == "CHAR")
        return is_const ? Const_Char : Char;

    if(type_string == "0record" || type_string == "0array")
    {
        if(bison_verbose)
        {
            cerr << "unsupported / unimplemented type string " << type_string << " on line: " << yylineno << endl;
            exit(1);
        }
    }
    else
    {
        //cerr << "unsupported / unimplemented type string " << type_string << " on line: " << yylineno << endl;
        //exit(1);
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

    //if(nonconst_val(lhs) == nonconst_val(rhs))
    return true;
}

Type::ValueType Type::nonconst_val(ValueType type)
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

Type::ValueType Type::const_val(ValueType type)
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
