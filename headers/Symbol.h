#ifndef __SYMBOL_H__
#define __SYMBOL_H__

#include <string>

#include "Type.h"
#include "ListWrapper.h"
#include "SymbolTable.h"

class Symbol
{
    public:
        Symbol(std::string& name, int offset, Type *type, bool global = true);
        Symbol(std::string& name, int offset, Type *type, Register* _reg, bool global);

        //const constructors
        Symbol(std::string& str_value)
            : name(GetLabel("cstr"))
            , str_value(str_value)
            , global(true)
            , rp(NULL)
        { type = SymbolTable::instance()->findType(Type::constStringId()); }

        Symbol(int v)
            : name(GetLabel("cint"))
            , int_value(v)
            , global(true)
            , rp(NULL)
        { type = SymbolTable::instance()->findType(Type::constIntegerId()); }

        Symbol(char c, std::string& s)
            : name(GetLabel("cchr"))
            , str_value(s)
            , char_value(c)
            , global(true)
            , rp(NULL)
        { type = SymbolTable::instance()->findType(Type::constCharId()); }

        Symbol(const std::string& name, bool b)
            : name(name)
            , bool_value(b)
            , global(true)
            , rp(NULL)
        { type = SymbolTable::instance()->findType(Type::constBoolId()); }

        Symbol(Symbol* s);

        virtual ~Symbol() {};

        static std::string GetLabel(const std::string& prefix, bool gen = true);

        std::string toString();
        std::string reg();

        void read();
        void store();
        void setType(Type::ValueType type);

        std::string name;
        std::string str_value;
        int int_value;
        int offset;
        bool bool_value;
        char char_value;
        bool global;
        Type *type;
        Type *subType;
        std::string regPointer;
        Register *rp;
};

typedef ListWrapper<Symbol*> SymList;

#endif //__SYMBOL_H__
