#ifndef __SYMBOL_H__
#define __SYMBOL_H__

#include <string>

#include "Type.h"

class Symbol
{
    public:
        Symbol(std::string& name, int offset, Type::ValueType type)
            : name(name)
            , offset(offset)
            , type(type)
            , size(4) //one address size
        {}

        Symbol(std::string& str_value)
            : name(NewLabel("cstr"))
            , str_value(str_value)
            , type(Type::Const_String)
            , size(4) //one address size
        {}

        Symbol(std::string& str_value, Type::ValueType type, int offset = -1);

        Symbol(int v)
            : name(NewLabel("cint"))
            , int_value(v)
            , type(Type::Const_Integer)
            , size(4)
        {}

        Symbol(char c, std::string& s)
            : name(NewLabel("cchr"))
            , str_value(s)
            , char_value(c)
            , type(Type::Const_Char)
            , size(1)
        {}

        Symbol(const std::string& name, bool b)
            : name(name)
            , bool_value(b)
            , type(Type::Const_Bool)
            , size(1)
        {}

        Symbol(Symbol* s);

        virtual ~Symbol() {};

        static std::string NewLabel(const std::string& prefix);

        std::string toString();

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
        int size;

        //TODO: add scope to the symbol
};

#endif //__SYMBOL_H__
