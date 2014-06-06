#ifndef __SYMBOL_TABLE_LEVEL_H__
#define __SYMBOL_TABLE_LEVEL_H__

#include <string>
#include <map>

#include "Type.h"

class Symbol;

class SymbolTableLevel
{
    public:
        SymbolTableLevel(bool isGlobal)
            : offset(0)
            , globalScope(isGlobal)
        {}

        void addVariable(std::string id, Type::ValueType type);
        Symbol* lookupVariable(std::string id);
        void addType(std::string id, Type::ValueType type);
        Type::ValueType lookupType(std::string id);

    private:
        void checkId(std::string id);

    private:
        std::map<std::string, Symbol*> variables;
        std::map<std::string, Type::ValueType> types;
        int offset;
        bool globalScope;

};

#endif //__SYMBOL_TABLE_LEVEL_H__
