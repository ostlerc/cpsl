#include "SymbolTableLevel.h"
#include "SymbolTable.h"
#include "Expression.h"
#include "Type.h"
#include "Symbol.h"
#include "Log.h"

#include <iostream>

using namespace std;

extern int yylineno;
extern int bison_verbose;
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
    variables[id] = new Symbol(id, -1, Type::typeProcedure(), true);
    //I am using this bool_value as a flag if the procedure has been declared or not
    variables[id]->bool_value = false;
    return variables[id];
}

Symbol* SymbolTableLevel::addFunction(std::string id, Type *returnType)
{
    variables[id] = new Symbol(id, -1, Type::typeFunction(), true);
    //I am using this bool_value as a flag if the procedure has been declared or not
    variables[id]->bool_value = false;
    variables[id]->subType = returnType;
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
    if(!type->isConst())
    {
        offset += size;
        cpsl_log->out << "\tadd $sp $sp " << size << " # popping " << id << "(" << offset << ") on line " << yylineno << endl;
    }
}

void SymbolTableLevel::add_const(std::string id, Symbol *s)
{
    checkId(id);
    if(!s->type->isConst())
    {
        cerr << "attempt to store non const variable on line " << yylineno << endl;
        exit(1);
    }

    s->name = id;
    variables[id] = s;
}

void SymbolTableLevel::cleanup()
{
    if(globalScope)
        return;

    for(auto rit = variables.rbegin(); rit != variables.rend(); rit++)
    {
        popVariable(rit->first, rit->second->type);
    }

}

Symbol* SymbolTableLevel::addVariable(std::string id, Type *type, bool named)
{
    checkId(id);

    Symbol *sym = new Symbol(id, offset, type, globalScope);
    int size = type->size;

    cpsl_log->out << "#added variable(" << id << ") global?" << globalScope << endl;

    if(!type->isConst())
    {
        if(globalScope && named)
        {
            cpsl_log->out << "\t.space " << size << " # " << id << "(" << offset << ")" << endl;
            offset += size;
        }
        else
        {
            cpsl_log->out << "\tadd $sp $sp " << -size << " # " << id << "(" << offset << ") on line " << yylineno << endl;
            offset -= size;
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
        if(bison_verbose)
            cpsl_log->out << "#argument " << e->toString() << endl;
        e->store(t_offset, "$sp", false);
        t_offset -= e->getSymbol()->type->nonconst_val()->size;
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

Type* SymbolTableLevel::addType(std::string id, Type::ValueType type, int size, bool _const, Type* nc)
{
    checkId(id);
    types[id] = new Type(id, type, size, _const, nc);
    return types[id];
}

void SymbolTableLevel::linkType(std::string id, Type* t)
{
    types[id] = t;
}
