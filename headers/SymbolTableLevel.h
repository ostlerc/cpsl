#ifndef __SYMBOL_TABLE_LEVEL_H__
#define __SYMBOL_TABLE_LEVEL_H__

#include <string>
#include <map>

#include "SymbolTable.h"
#include "Type.h"

class Symbol;

class SymbolTableLevel
{
    public:
        SymbolTableLevel(bool isGlobal)
            : offset(0)
            , globalScope(isGlobal)
        {}

        void loadParams(std::vector<Parameters> params);
        void unloadParams(std::vector<Parameters> params);
        void saveExpressions(std::vector<Expression*> expr_list);

        void add_const(std::string id, Symbol *s);
        Symbol* addVariable(std::string id, Type *type, bool named = true);
        void popVariable(std::string id, Type *type);
        Symbol* addProcedure(std::string id);
        Symbol* addFunction(std::string id, Type *returnType);
        void checkProcedures();
        Symbol* lookupVariable(std::string id);
        Type* lookupType(std::string id);
        Type* addType(std::string id, Type::ValueType type, int size, bool _const = false, Type* nc = NULL);


    private:
        void checkId(std::string id);

    private:
        std::map<std::string, Symbol*> variables;
        std::map<std::string, Type*> types;
        int offset;
        bool globalScope;

};

#endif //__SYMBOL_TABLE_LEVEL_H__
