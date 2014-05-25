#ifndef __SYMBOL_H__
#define __SYMBOL_H__

#include <string>

class Symbol
{
    public:
        Symbol(std::string& name, int value, int offset)
            : name(name)
            , value(value)
            , offset(offset) {}

        Symbol(std::string& str_value)
            : name(NewLabel())
            , value(0)
            , str_value(str_value)
            , offset(0) {}

        static std::string NewLabel();

        virtual ~Symbol() {};

        const std::string str_val();

        void read(); //TODO: for now only read integers, in the future enhance.

        std::string name;
        int value;
        std::string str_value;
        int offset;
};

#endif //__SYMBOL_H__
