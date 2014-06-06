#include "SymbolTableLevel.h"
#include "Expression.h"
#include "Type.h"
#include "Symbol.h"

#include <iostream>

using namespace std;

extern int yylineno;

void SymbolTableLevel::checkId(std::string id)
{
    auto var = variables.find(id);
    if(var != variables.end())
    {
        cerr << id << " already defined as variable";
        exit(1);
    }

    auto type = types.find(id);
    if(type != types.end())
    {
        cerr << id << " already defined as variable";
        exit(1);
    }
}

void SymbolTableLevel::addVariable(std::string id, Type::ValueType type)
{
    checkId(id);

    Symbol *sym = new Symbol(id, offset, type, globalScope);

    //TODO: add size by type
    int size = 4;

    if(!Type::isConst(type))
    {
        offset += 4*(globalScope ? 1: -1);
        cout << "\t.space " << size << " # " << id << "(" << offset << ")" << endl;
    }

    variables[id] = sym;
}

Symbol* SymbolTableLevel::lookupVariable(std::string id)
{
    auto var = variables.find(id);
    if(var == variables.end()) { cerr << "failed to lookup " << id << " on line " << yylineno << endl; exit(1); }
    return var->second;
}

void SymbolTableLevel::addType(std::string id, Type::ValueType type)
{
    checkId(id);
    types[id]=type;
}

Type::ValueType SymbolTableLevel::lookupType(std::string id)
{
    return Type::fromString(id);
}
