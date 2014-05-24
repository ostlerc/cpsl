#include "Expression.h"

#include <iostream>

using namespace std;

Expression* Expression::unimp(Expression* e)
{
    cerr << "Unimplemented function " << __FUNCTION__ << endl;
    exit(1);
}

Expression* Expression::plus(Expression* e)
{
    if(is_const && e->is_const && type == INT && e->type == INT)
    {
        cout << "const adding " << const_val << "+" << e->const_val << endl;
        const_val += e->const_val;
        delete e;
    }
    else if(type == INT && e->type == INT)
    {
        if(!is_const && !symbol)
        {
            cerr << "lhs missing symbol on expression " << endl;
            exit(1);
        }

        if(!e->is_const && !e->symbol)
        {
            cerr << "rhs missing symbol on expression " << endl;
            exit(1);
        }

        cout << "here " << is_const << " " << symbol << endl;
        int v = is_const ? const_val : symbol->value;
        cout << "here" << endl;
        int ev = e->is_const ? e->const_val : e->symbol->value;
        cout << "here" << endl;
        return new Expression(v + ev);
    }
    else
    {
        cerr << "unimp add with non const" << endl;
        exit(1);
    }

    return this;
}

Expression* Expression::mul(Expression* e)
{
    if(is_const && e->is_const && type == INT && e->type == INT)
    {
        cout << "const multiplying " << const_val << "*" << e->const_val << endl;
        const_val *= e->const_val;
        delete e;
    }
    else
    {
        cerr << "unimp mul with non const" << endl;
        exit(1);
    }

    return this;
}

Expression* Expression::sub(Expression* e)
{
    if(is_const && e->is_const && type == INT && e->type == INT)
    {
        cout << "const subtracting " << const_val << "-" << e->const_val << endl;
        const_val -= e->const_val;
        delete e;
    }
    else
    {
        cerr << "unimp sub with non const" << endl;
        exit(1);
    }

    return this;
}

Expression* Expression::div(Expression* e)
{
    if(is_const && e->is_const && type == INT && e->type == INT)
    {
        cout << "const dividing " << const_val << "/" << e->const_val << endl;
        const_val /= e->const_val;
        delete e;
    }
    else
    {
        cerr << "unimp div with non const" << endl;
        exit(1);
    }

    return this;
}

Expression* Expression::mod(Expression* e)
{
    if(is_const && e->is_const && type == INT && e->type == INT)
    {
        cout << "const mod'ing " << const_val << "%" << e->const_val << endl;
        const_val = const_val % e->const_val;
        delete e;
    }
    else
    {
        cerr << "unimp mod with non const" << endl;
        exit(1);
    }

    return this;
}
