#include <string>
#include <iostream>

#include "Symbol.h"
#include "SymbolTable.h"
#include "Register.h"

using namespace std;

extern bool bison_verbose;

std::string Symbol::NewLabel()
{
    static int at = 0;
    return "cstr" + to_string(at++);
}

const std::string Symbol::str_val()
{
    if(str_value.length() > 0)
        return str_value;
    else
        return to_string(value);
}

void Symbol::read()
{
    if(bison_verbose)
        cout << "\treading symbol " << name << endl;

    cout << "\tli $v0, 5 #read var " << name << endl;
    cout << "\tsyscall" << endl;

    Register *reg = Register::FindRegister(Register::Temp);

    cout << "\tla " << reg->name() << " dat" << endl;
    cout << "\tsw $v0, " << offset << "(" << reg->name() << ")" << endl;

    Register::ReleaseRegister(reg);
}
