#ifndef __SYMBOL_H__
#define __SYMBOL_H__

#include <string>

#include "Type.h"

class Symbol
{
    public:
        Symbol(std::string& name, int offset, Type::ValueType type, bool global = true)
            : name(name)
            , offset(offset)
            , type(type)
            , global(global)
        {}

        //const constructors
        Symbol(std::string& str_value)
            : name(GetLabel("cstr"))
            , str_value(str_value)
            , type(Type::Const_String)
            , global(true)
        {}

        Symbol(int v)
            : name(GetLabel("cint"))
            , int_value(v)
            , type(Type::Const_Integer)
            , global(true)
        {}

        Symbol(char c, std::string& s)
            : name(GetLabel("cchr"))
            , str_value(s)
            , char_value(c)
            , type(Type::Const_Char)
            , global(true)
        {}

        Symbol(const std::string& name, bool b)
            : name(name)
            , bool_value(b)
            , type(Type::Const_Bool)
            , global(true)
        {}

        Symbol(Symbol* s);

        virtual ~Symbol() {};

        static std::string GetLabel(const std::string& prefix, bool gen = true);

        std::string toString();
        std::string reg() { return global ? "$gp" : "$fp"; }

        void read(); //TODO: for now only read integers, in the future enhance.
        void store();
        void setType(Type::ValueType type);

        std::string name;
        std::string str_value;
        int int_value;
        int offset;
        bool bool_value;
        char char_value;
        Type::ValueType type;
        bool global;
};

#endif //__SYMBOL_H__
