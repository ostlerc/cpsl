#include "SymbolTableLevel.h"
#include "SymbolTable.h"
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
        cerr << id << " already defined as variable on line " << yylineno << endl;
        exit(1);
    }

    auto type = types.find(id);
    if(type != types.end())
    {
        cerr << id << " already defined as variable on line " << yylineno << endl;
        exit(1);
    }
}

Symbol* SymbolTableLevel::addProcedure(std::string id)
{
    variables[id] = new Symbol(id, -1, Type::Procedure);
    variables[id]->bool_value = false;
    return variables[id];
}

void SymbolTableLevel::checkProcedures()
{
    for(auto& v : variables)
    {
        Symbol *sym = v.second;
        if((sym->type == Type::Procedure || sym->type == Type::Function) && !sym->bool_value)
        {
            cerr << "unresolved symbol " << sym->toString() << endl;
            exit(1);
        }
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

        if(globalScope)
        {
            offset += size;
            cout << "\t.space " << size << " # " << id << "(" << offset << ")" << endl;
        }
        else
        {
            offset -= size;
            //TODO: make this in prologue
            cout << "\tadd $sp $sp " << -size << " # " << id << "(" << offset << ") on line " << yylineno << endl;
        }
    }

    variables[id] = sym;
}

Symbol* SymbolTableLevel::lookupVariable(std::string id)
{
    auto var = variables.find(id);
    if(var == variables.end()) return NULL;
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

void SymbolTableLevel::load()
{
}

void SymbolTableLevel::store()
{
}
