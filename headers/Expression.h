#ifndef __EXPRESSION_H__
#define __EXPRESSION_H__

#include "Symbol.h"
#include "Register.h"

class Expression
{
    public:
        Expression(int v) 
            : type(INT)
            , const_int(v)
            , symbol(NULL)
            , reg(NULL)
        {}

        Expression(Symbol *s) 
            : type(SYM)
            , const_int(1)
            , symbol(s)
            , reg(NULL)
        {}

        Expression(std::string &name)
            : type(STR)
            , const_int(1)
            , const_str(name)
            , symbol(NULL)
            , reg(NULL)
        {}

        virtual ~Expression() {}

        Expression* unimp(Expression* e);
        Expression* plus(Expression* e);
        Expression* mul(Expression* e);
        Expression* sub(Expression* e);
        Expression* div(Expression* e);
        Expression* mod(Expression* e);

    private:
        enum ExpressionType
        {
            INT,
            STR,
            SYM
        };

        int getInt();
        bool canFold(Expression* e);
        std::string toString();
        std::string typeName();

        ExpressionType type; 
        int const_int;
        std::string const_str;
        Symbol* symbol;
        Register* reg;
};

#endif //__EXPRESSION_H__
