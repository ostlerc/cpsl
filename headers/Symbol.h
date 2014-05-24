#ifndef __SYMBOL_H__
#define __SYMBOL_H__

#include <string>

class Symbol
{
    public:
        Symbol(std::string name, int value, int offset) 
            : name(name)
            , value(value)
            , offset(offset) {}

        virtual ~Symbol() {};

        std::string name;
        int value;
        int offset;
};

#endif //__SYMBOL_H__
