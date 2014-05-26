#include "Type.h"

#include <iostream>

using namespace std;

extern int yylineno;

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

Type::ValueType Type::fromString(std::string type_string)
{
    if(type_string == "integer")
        return Integer;

    if(type_string == "0record" || type_string == "0array")
    {
        cerr << "unsupported / unimplemented type string " << type_string << " on line: " << yylineno << endl;
    }
    exit(1);
}
