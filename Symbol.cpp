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

std::string Symbol::NewLabel(const string& prefix)
{
    static int at = 0;
    return prefix + to_string(at++);
}

void Symbol::read()
{
    if(bison_verbose)
        cout << "\treading symbol " << toString() << endl;

    cout << "\tli $v0, 5 #read symbol " << toString() << " on line: " << yylineno << endl;
    cout << "\tsyscall" << endl;

    cout << "\tsw $v0, " << offset << "($gp)" << endl;
}

string Symbol::toString()
{
    string o = Type::toString(type);
    o += " " + name;

    switch(type)
    {
        case Type::Const_Integer:
            o += " val=" + to_string(int_value);
    }

    return o;
    //TODO: show const symbols values here
}
