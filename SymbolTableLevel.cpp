#include "SymbolTableLevel.h"
#include "SymbolTable.h"
#include "Expression.h"
#include "Type.h"
#include "Symbol.h"
#include "Log.h"

#include <iostream>

using namespace std;

extern int yylineno;
extern Log* cpsl_log;

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
    //I am using this bool_value as a flag if the procedure has been declared or not
    variables[id]->bool_value = false;
    return variables[id];
}

Symbol* SymbolTableLevel::addFunction(std::string id, Type::ValueType returnType)
{
    variables[id] = new Symbol(id, -1, Type::Function);
    //I am using this bool_value as a flag if the procedure has been declared or not
    variables[id]->bool_value = false;
    variables[id]->returnType = returnType;
    return variables[id];
}

void SymbolTableLevel::checkProcedures()
{
    for(auto& v : variables)
    {
        Symbol *sym = v.second;
        //I am using the symbol bool_value as a flag if the procedure has been declared or not
        if((sym->type == Type::Procedure || sym->type == Type::Function) && !sym->bool_value)
        {
            cerr << "unresolved symbol " << sym->toString() << endl;
            exit(1);
        }
    }
}

void SymbolTableLevel::loadParams(std::vector<Parameters> params)
{
    for(Parameters p : params)
    {
        for(string id : p.vars)
        {
            addVariable(id, p.type);
        }
    }
}

void SymbolTableLevel::unloadParams(std::vector<Parameters> params)
{
    for(Parameters p : params)
    {
        for(string id : p.vars)
        {
            popVariable(id, p.type);
        }
    }
}

void SymbolTableLevel::popVariable(std::string id, Type::ValueType type)
{
    //TODO: add size by type
    if(globalScope)
    {
        cerr << "cannot pop a global variable! on line " << yylineno << endl;
        exit(1);
    }

    int size = 4;
    if(!Type::isConst(type))
    {
        offset += size;
        cpsl_log->out << "\tadd $sp $sp " << size << " # popping " << id << "(" << offset << ") on line " << yylineno << endl;
    }
}

Symbol* SymbolTableLevel::addVariable(std::string id, Type::ValueType type, bool named)
{
    checkId(id);

    Symbol *sym = new Symbol(id, offset, type, globalScope);

    //TODO: add size by type
    int size = 4;

    if(!Type::isConst(type))
    {

        if(globalScope && named)
        {
            offset += size;
            cpsl_log->out << "\t.space " << size << " # " << id << "(" << offset << ")" << endl;
        }
        else
        {
            offset -= size;
            cpsl_log->out << "\tadd $sp $sp " << -size << " # " << id << "(" << offset << ") on line " << yylineno << endl;
        }
    }

    if(named)
        variables[id] = sym;

    return sym;
}

void SymbolTableLevel::saveExpressions(std::vector<Expression*> expr_list)
{
    int t_offset = 0;
    for(Expression *e : expr_list)
    {
        cpsl_log->out << "#argument " << e->toString() << endl;
        e->store(t_offset, "$sp");
        t_offset -= 4;
    }
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
