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

    auto tvar = types.find(id);
    if(tvar != types.end())
    {
        cerr << id << " already defined as type on line " << yylineno << endl;
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
    variables[id] = new Symbol(id, -1, Type::typeProcedure());
    //I am using this bool_value as a flag if the procedure has been declared or not
    variables[id]->bool_value = false;
    return variables[id];
}

Symbol* SymbolTableLevel::addFunction(std::string id, Type *returnType)
{
    variables[id] = new Symbol(id, -1, Type::typeFunction());
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
        if((sym->type->vt == Type::Procedure || sym->type->vt == Type::Function) && !sym->bool_value)
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

void SymbolTableLevel::popVariable(std::string id, Type* type)
{
    if(globalScope)
    {
        cerr << "cannot pop a global variable! on line " << yylineno << endl;
        exit(1);
    }

    int size = type->size;
    if(!Type::isConst(type->vt))
    {
        offset += size;
        cpsl_log->out << "\tadd $sp $sp " << size << " # popping " << id << "(" << offset << ") on line " << yylineno << endl;
    }
}

Symbol* SymbolTableLevel::addVariable(std::string id, Type *type, bool named)
{
    checkId(id);

    Symbol *sym = new Symbol(id, offset, type, globalScope);

    int size = type->size;

    if(!Type::isConst(type->vt))
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

Type* SymbolTableLevel::lookupType(std::string id)
{
    auto var = types.find(id);
    if(var == types.end()) return NULL;
    return var->second;
}

Type* SymbolTableLevel::addType(std::string id, Type::ValueType type, int size)
{
    checkId(id);
    types[id] = new Type(type, size);
    return types[id];
}
