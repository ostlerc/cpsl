#ifndef __EXPRESSION_H__
#define __EXPRESSION_H__

#include "Register.h"
#include "ListWrapper.h"
#include "Type.h"

class Symbol;
class SymbolTable;

class Expression
{
    friend SymbolTable;

    public:
        Expression(Symbol *s)
            : symbol(s)
            , reg(NULL)
        {}

        Expression(Expression* e);

        virtual ~Expression() {}

        Expression* unimp(Expression* e);

        void assign(Expression* e);

        enum Operation
        {
            Add,
            Sub,
            Mul,
            Div,
            Mod,
            Eq,
            Ne,
            Lt,
            Gt,
            Lte,
            Gte,
            Succ,
            Pred,
            Negate,
            Chr,
            Ord,
            Bar,
            Amp,
            Tilde
        };

        Expression* exec(Expression* e, Operation op);
        Expression* exec(Operation op);

        std::string toString();

        void print(); //print expression result to prompt

        void loadInTemp();
        void free(); //release temporary register

        void invalidType(Operation op);
        void setType(Operation op, bool isConst = true);
        void store(int offset = -1, std::string reg = "", bool reg_global = true);
        Type *type();
        Symbol *getSymbol() { return symbol; }
        std::string regStr() { return reg ? "" : reg->name(); }

    private:

        std::string toString(Operation op);

        bool canFold(Expression* e);
        bool canFold(Operation op);
        void setVal(int);
        std::string typeName();

        Symbol* symbol;
        Register* reg;
};

typedef ListWrapper<Expression*> ExprList;

#endif //__EXPRESSION_H__
