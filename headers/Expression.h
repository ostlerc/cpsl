#ifndef __EXPRESSION_H__
#define __EXPRESSION_H__

#include "Symbol.h"
#include "Register.h"

class Expression
{
    public:
        //TODO: remove types from expression and put them in symbol
        //const integers
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

        Expression(Register *reg)
            : type(SYM)
              , const_int(1)
              , symbol(NULL)
              , reg(reg)
        {}

        //const strings
        Expression(std::string &name)
            : type(STR)
            , const_int(1)
            , symbol(new Symbol(name))
            , reg(NULL)
        {}

        virtual ~Expression() {}

        Expression* unimp(Expression* e);
        Expression* plus(Expression* e);
        Expression* mul(Expression* e);
        Expression* sub(Expression* e);
        Expression* div(Expression* e);
        Expression* mod(Expression* e);

        std::string toString();

        void print(); //print expression result to prompt
        void store(); //write out const string data to .data section

        void loadInTemp();

    private:
        enum ExpressionType
        {
            INT,
            STR,
            SYM
        };

        int getInt();
        bool canFold(Expression* e);
        std::string typeName();

        ExpressionType type;
        int const_int;
        Symbol* symbol;
        Register* reg;
};

#endif //__EXPRESSION_H__