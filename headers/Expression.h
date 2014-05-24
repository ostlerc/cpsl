#ifndef __EXPRESSION_H__
#define __EXPRESSION_H__

#include "Symbol.h"
#include "Register.h"

class Expression
{
    public:
        Expression(int v) 
            : is_const(true)
            , type(INT)
            , const_val(v)
            , symbol(NULL)
            , reg(NULL)
        {}

        Expression(Symbol *s) 
            : is_const(false)
            , type(SYM)
            , const_val(0)
            , symbol(s)
            , reg(NULL)
        {}

        Expression(std::string &name)
            : is_const(true)
            , type(STR)
            , const_val(0)
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

        bool is_const;
        ExpressionType type; 
        int const_val;
        Symbol* symbol;
        Register* reg;
};

#endif //__EXPRESSION_H__
