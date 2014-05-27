#include "Expression.h"

#include <iostream>

using namespace std;

extern int yylineno;// defined and maintained in lex.cpp
extern bool bison_verbose;

Expression* Expression::unimp(Expression* e)
{
    return e;
    //cerr << "Unimplemented function " << __FUNCTION__ << " line: " << yylineno << endl;
    //exit(1);
}

string Expression::toString(Operation op)
{
    switch(op)
    {
        case Add:
              return "add";
        case Sub:
              return "subtract";
        case Mul:
              return "multiply";
        case Div:
              return "divide";
        case Mod:
              return "mod";
        case Eq:
              return "equal";
        case Ne:
              return "not equal";
        case Lt:
              return "less than";
        case Gt:
              return "greater than";
        case Lte:
              return "less than equal";
        case Gte:
              return "greater than equal";
        case Succ:
              return "succ";
        case Pred:
              return "pred";
        case Negate:
              return "negate";
        case Chr:
              return "chr";
        case Ord:
              return "ord";
        case Bar:
              return "bar";
        case Amp:
              return "amp";
        case Tilde:
              return "tilde";
        default:
              cerr << "Unknown op " << op << " in function " << __FUNCTION__ << " line: " << yylineno << endl;
              exit(1);
    }
}

Expression* Expression::exec(Expression* e, Operation op)
{
    if(bison_verbose)
        cout << "exec on " << toString() << " " << toString(op) << " " << e->toString() << " on line: " << yylineno << endl;

    int rhs_v = 0;
    switch(e->symbol->type)
    {
        case Type::Integer:
        case Type::Const_Integer:
            rhs_v = e->symbol->int_value;
            break;
        case Type::Bool:
        case Type::Const_Bool:
            rhs_v = e->symbol->bool_value;
        case Type::Const_Char:
            rhs_v = e->symbol->char_value;
            break;
        case Type::Unknown:
            return this; //skip unknown types for now
        default:
            cerr << "unhandled (rhs) type error on line " << yylineno << " " << e->toString() << "-" << e->toString() << endl;
    }

    if(canFold(e))
    {
        int lhs_v = 0;
        switch(symbol->type)
        {
            case Type::Integer:
            case Type::Const_Integer:
                lhs_v = symbol->int_value;
                break;
            case Type::Bool:
            case Type::Const_Bool:
                lhs_v = symbol->bool_value;
                break;
            case Type::Const_Char:
                lhs_v = symbol->char_value;
                break;
            default:
                cerr << "unhandled (lhs) type error on line " << yylineno << " " << toString() << "-" << e->toString() << endl;
                exit(1);
        }

        if(bison_verbose)
            cout << toString() << " " << toString(op) << " folding " << e->toString() << ", lhs=" << lhs_v << ", rhs=" << rhs_v << " line: " << yylineno << endl;

        switch(op)
        {
            case Add:
                setVal(lhs_v + rhs_v);
                break;
            case Sub:
                setVal(lhs_v - rhs_v);
                break;
            case Mul:
                setVal(lhs_v * rhs_v);
                break;
            case Div:
                setVal(lhs_v / rhs_v);
                break;
            case Mod:
                setVal(lhs_v % rhs_v);
                break;
            case Eq:
                setVal(lhs_v == rhs_v);
                break;
            case Ne:
                setVal(lhs_v != rhs_v);
                break;
            case Lt:
                setVal(lhs_v < rhs_v);
                break;
            case Gt:
                setVal(lhs_v > rhs_v);
                break;
            case Lte:
                setVal(lhs_v <= rhs_v);
                break;
            case Gte:
                setVal(lhs_v >= rhs_v);
                break;
            case Bar:
                setVal(lhs_v | rhs_v);
                break;
            case Amp:
                setVal(lhs_v & rhs_v);
                break;
            default:
                {
                    cerr << "unsupported binary operator " << op << " line: " << yylineno << endl;
                    exit(1);
                }
        }
        return this;
    }
    switch(symbol->type)
    {
        case Type::Integer:
        case Type::Const_Integer:
        case Type::Bool:
        case Type::Char:
        case Type::Const_Char:
            {
                loadInTemp();

                string rhs_str;
                if(!Type::isConst(e->symbol->type))
                {
                    e->loadInTemp();
                    rhs_str = e->reg->name();
                }
                else
                {
                    switch(op)
                    {
                        case Add:
                        case Sub:
                        case Mul:
                        case Div:
                        case Mod:
                        case Eq:
                        case Ne:
                            rhs_str = to_string(rhs_v);
                            break;
                        case Lt:
                        case Gt:
                        case Lte:
                        case Gte:
                        case Bar:
                        case Amp:
                            e->loadInTemp();
                            rhs_str = e->reg->name();
                            break;
                        default:
                            {
                                cerr << "Incorrect binary operator on line " << yylineno << endl;
                                exit(1);
                            }
                    }
                }

                switch(op)
                {
                    case Add:
                        cout << "\tadd " << reg->name() << ", " << reg->name() << ", " << rhs_str << endl;
                        break;
                    case Sub:
                        cout << "\tsub " << reg->name() << ", " << reg->name() << ", " << rhs_str << endl;
                        break;
                    case Mul:
                        cout << "\tmul " << reg->name() << ", " << reg->name() << ", " << rhs_str << endl;
                        break;
                    case Div:
                        cout << "\tdiv " << reg->name() << ", " << reg->name() << ", " << rhs_str << endl;
                        break;
                    case Mod:
                        cout << "\tdiv " << reg->name() << ", " << reg->name() << ", " << rhs_str << endl;
                        cout << "\tmfhi " << reg->name() << " # get quotient of div" << endl;
                        break;
                    case Eq:
                        cout << "\tseq " << reg->name() << ", " << reg->name() << ", " << rhs_str << endl;
                        break;
                    case Ne:
                        cout << "\tsne " << reg->name() << ", " << reg->name() << ", " << rhs_str << endl;
                        break;
                    case Lt:
                        cout << "\tslt " << reg->name() << ", " << reg->name() << ", " << rhs_str << endl;
                        break;
                    case Gt:
                        cout << "\tsgt " << reg->name() << ", " << reg->name() << ", " << rhs_str << endl;
                        break;
                    case Lte:
                        cout << "\tsle " << reg->name() << ", " << reg->name() << ", " << rhs_str << endl;
                        break;
                    case Gte:
                        cout << "\tsge " << reg->name() << ", " << reg->name() << ", " << rhs_str << endl;
                        break;
                    case Bar:
                        cout << "\tor " << reg->name() << ", " << reg->name() << ", " << rhs_str << endl;
                        break;
                    case Amp:
                        cout << "\tand " << reg->name() << ", " << reg->name() << ", " << rhs_str << endl;
                        break;
                    default:
                        {
                            cerr << "Incorrect binary operator on line " << yylineno << endl;
                            exit(1);
                        }
                }

                if(e->reg)
                    e->free();

                Expression *exp = new Expression(new Symbol(symbol));
                exp->reg = reg;
                exp->setType(op);
                reg = NULL;

                return exp;
            }
            break;
        default:
            {
                cerr << "Unimplemented or incorrect exec type " << toString() << "-" << e->toString() << " line: " << yylineno << endl;
                exit(1);
            }
            break;
    }

    return this;
}

Expression* Expression::exec(Operation op)
{
    switch(op)
    {
        case Pred:
            {
                Expression *e = new Expression(new Symbol(1));
                return exec(e, Sub);
            }
            break;
        case Succ:
            {
                Expression *e = new Expression(new Symbol(1));
                return exec(e, Add);
            }
            break;
        case Negate:
            {
                if(Type::isConst(symbol->type))
                {
                    switch(symbol->type)
                    {
                        case Type::Const_Integer:
                            symbol->int_value = -symbol->int_value;
                            break;
                        case Type::Const_Bool:
                            symbol->bool_value = -symbol->bool_value;
                            break;
                        default:
                            invalidType(op);
                            break;
                    }
                }
                else
                {
                    loadInTemp();
                    cout << "\tneg " << reg->name() << ", " << reg->name() << " #negate " << symbol->name << " on line: " << yylineno << endl;
                }
            }
            break;
        case Chr:
        case Ord:
            if(bison_verbose)
                cout << "setting type for " << toString() << " on line: " << yylineno << endl;
            setType(op);
            break;
        case Tilde:
            {
                if(Type::isConst(symbol->type))
                {
                    switch(symbol->type)
                    {
                        case Type::Const_Integer:
                            symbol->int_value = ~symbol->int_value;
                            break;
                        case Type::Const_Bool:
                            symbol->bool_value = ~symbol->bool_value;
                            break;
                        default:
                            invalidType(op);
                            break;
                    }
                }
                else
                {
                    loadInTemp();
                    cout << "\tnot " << reg->name() << ", " << reg->name() << " #bitwise NOT " << symbol->name << " on line: " << yylineno << endl;
                }
            }
            break;
        default:
            {
                cerr << "Unsupported unary expression operator " << op << " line: " << yylineno << endl;
                exit(1);
            }
    }

    return this;
}

void Expression::invalidType(Operation op)
{
    cerr << "invalid operation (" << toString(op) << " for " << toString() << " on line: " << yylineno << endl;
    exit(1);
}

bool Expression::canFold(Expression* e)
{

    return Type::isConst(symbol->type) && Type::isConst(e->symbol->type) &&
           Type::isFoldable(symbol->type) && Type::isFoldable(e->symbol->type);
}

string Expression::toString()
{
    string o = symbol->toString();

    if(reg)
        o += " in register " + reg->name();

    return o;
}

void Expression::print()
{
    if(bison_verbose)
        cout << "printing expression " << toString() << endl;

    switch(symbol->type)
    {
        case Type::Const_Integer:
            {
                cout << "\tli $v0, 1" << endl;
                cout << "\tli $a0, " << symbol->int_value << " #" << toString() << " line: " << yylineno << endl;
            }
            break;
        case Type::Const_String:
        case Type::Const_Char:
            {
                cout << "\tli $v0, 4" << endl;
                cout << "\tla $a0, " << symbol->name << " # " << symbol->str_value << " line: " << yylineno << endl;
            }
            break;
        case Type::Integer:
            {
                cout << "\tli $v0 1" << endl;
                if(reg)
                {
                    cout << "\tadd $a0, " << reg->name() << ", 0 # Set printing register for symbol " << symbol->toString() << endl;
                }
                else
                {
                    cout << "\tlw $a0, " << symbol->offset << "($gp)" << endl;
                }
            }
            break;
        case Type::Bool:
            {
                cout << "\tli $v0 1" << endl;
                if(reg)
                {
                    cout << "\tsne $a0, " << reg->name() << ", 0 # Set printing register for symbol " << symbol->toString() << endl;
                }
                else
                {
                    cout << "\tlb $a0, " << symbol->offset << "($gp)" << endl;
                    cout << "\tsne $a0, $a0, 0 # Boolean var set only to 1 or 0" << endl;
                }
            }
            break;
        case Type::Char:
            {
                cout << "\tli $v0 11" << endl; //print character
                if(reg)
                {
                    cout << "\tadd $a0, " << reg->name() << ", 0 # Set printing register for symbol " << symbol->toString() << endl;
                }
                else
                {
                    cout << "\tlw $a0, " << symbol->offset << "($gp)" << endl;
                }
            }
            break;
        case Type::Const_Bool:
            {
                cout << "\tli $v0, 1" << endl;
                cout << "\tli $a0, " << symbol->bool_value << " #" << toString() << " line: " << yylineno << endl;
            }
            break;
        case Type::Unknown:
            {
                //skip unknown for now
            }
            break;
        default:
            {
                cerr << "Unimplemented print type " << toString() << " on line: " << yylineno << endl;
                exit(1);
            }
        break;
            break;
    }

    cout << "\tsyscall" << endl;
    free();
}

void Expression::loadInTemp()
{
    if(reg)//already done!
        return;

    reg = Register::FindRegister(Register::Temp);

    switch(symbol->type)
    {
        case Type::Integer:
            {
                cout << "\tlw " << reg->name() << ", " << symbol->offset << "($gp) "
                    << "#Loading symbol " << symbol->name << " into reg " << reg->name() << " on line: " << yylineno << endl;
            }
            break;
        case Type::Bool:
        case Type::Char:
            {
                cout << "\tlb " << reg->name() << ", " << symbol->offset << "($gp) "
                    << "#Loading symbol " << symbol->name << " into reg " << reg->name() << " on line: " << yylineno << endl;
            }
            break;
        case Type::Const_Integer:
        case Type::Const_Bool:
            {
                cout << "\tli " << reg->name() << ", " << symbol->int_value
                    << "#Loading const integer into reg " << reg->name() << " on line: " << yylineno << endl;
            }
            break;
        case Type::Const_String:
            {
                cout << "\tla " << reg->name() << ", " << symbol->name
                    << " # Loading " << toString() << " on line: " << yylineno << endl;
            }
            break;
        case Type::Const_Char:
            {
                cout << "\tli " << reg->name() << ", " << (int)symbol->char_value
                    << " # Loading " << toString() << " on line: " << yylineno << endl;
            }
            break;
        case Type::Unknown:
            {
                //Skip unknown for now
            }
            break;
        default:
            {
                cerr << "Attempting to load expression (" << toString() << ") into a temp register is invalid or unimplemented. line: " << yylineno << endl;
                exit(1);
            }
    }
}

void Expression::free()
{
    if(reg)
    {
        Register::ReleaseRegister(reg);
        reg = NULL;
    }
}

void Expression::assign(Symbol* s)
{
    if(!Type::match(symbol->type, s->type))
    {
        cerr << "Expression type mismatch: " 
            << toString() << "-"
            << s->toString() << " on line " << yylineno << endl;
        exit(1);
    }

    //s is actually the lhs
    if(Type::isConst(s->type))
    {
        if(bison_verbose)
        {
            cerr << "const lValue (" << toString() << ") assignment not allowed on line " << yylineno << endl;
            exit(1);
        }
        return;
    }

    loadInTemp();
    switch(s->type)
    {
        case Type::Bool:
        case Type::Char:
            {
                cout << "\tsb " << reg->name() << ", " << s->offset << "($gp) #Assign var (" << s->toString() << ") to (" << toString() << ") on line: " << yylineno << endl;
            }
            break;
        default:
            {
                cout << "\tsw " << reg->name() << ", " << s->offset << "($gp) #Assign var (" << s->toString() << ") to (" << toString() << ") on line: " << yylineno << endl;
            }
        break;
    }

    Register::ReleaseRegister(reg);
    reg = NULL;
}

void Expression::setType(Operation op)
{
    switch(op)
    {
        case Add:
        case Sub:
        case Mul:
        case Div:
        case Mod:
            symbol->setType(Type::Integer);
            break;
        case Eq:
        case Ne:
        case Lt:
        case Gt:
        case Lte:
        case Gte:
            symbol->setType(Type::Bool);
            break;
        case Chr:
            symbol->setType(Type::Char);
            break;
        case Ord:
            symbol->setType(Type::Integer);
            break;
        default:
            break;
    }
}

void Expression::setVal(int v)
{
    if(bison_verbose)
        cout << "setting value " << v << " for " << toString() << " on line " << yylineno << endl;

    switch(symbol->type)
    {
        case Type::Const_Integer:
            symbol->int_value = v;
            break;
        case Type::Const_Bool:
            symbol->bool_value = v == 0 ? false : true;
            break;
        case Type::Char:
            symbol->char_value = (char)v;
            break;
        default:
            {
                cerr << "Attempting to set incorrect or unimplemented const value " << toString() << " on line " << yylineno << endl;
                exit(1);
            }
    }
}
