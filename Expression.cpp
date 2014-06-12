#include "Expression.h"
#include "Log.h"
#include "Symbol.h"

#include <iostream>

using namespace std;

extern int yylineno;// defined and maintained in lex.cpp
extern bool bison_verbose;
extern Log* cpsl_log;

Expression::Expression(Expression* e)
    : symbol(new Symbol(e->symbol))
    , reg(e->reg)
{ e->reg = NULL; }

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
    //TODO: remove me
    if(e->symbol->type->vt == Type::Unknown || symbol->type->vt == Type::Unknown)
        return this;

    if(bison_verbose)
        cpsl_log->out << "exec on " << toString() << " " << toString(op) << " " << e->toString() << " on line: " << yylineno << endl;

    int rhs_v = 0;
    switch(e->symbol->type->vt)
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
            //return this; //skip unknown types for now
        default:
            if(bison_verbose)
            {
                cerr << "unhandled (rhs) type error on line " << yylineno << " " << toString() << "-" << e->toString() << endl;
                exit(1);
            }
    }

    if(canFold(op) && canFold(e))
    {
        int lhs_v = 0;
        switch(symbol->type->vt)
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
            cpsl_log->out << toString() << " " << toString(op) << " folding " << e->toString() << ", lhs=" << lhs_v << ", rhs=" << rhs_v << " line: " << yylineno << endl;

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
                setVal(lhs_v || rhs_v);
                break;
            case Amp:
                setVal(lhs_v && rhs_v);
                break;
            default:
                {
                    cerr << "unsupported binary operator " << op << " line: " << yylineno << endl;
                    exit(1);
                }
        }
        return this;
    }
    switch(symbol->type->vt)
    {
        case Type::Integer:
        case Type::Const_Integer:
        case Type::Bool:
        case Type::Char:
        case Type::Const_Char:
            {
                loadInTemp();

                string rhs_str;
                if(!Type::isConst(e->symbol->type->vt))
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
                                cerr << "Incorrect binary operator " << toString() << " " << toString(op) << " on line " << yylineno << endl;
                                exit(1);
                            }
                    }
                }

                switch(op)
                {
                    case Add:
                        cpsl_log->out << "\tadd " << reg->name() << ", " << reg->name() << ", " << rhs_str << endl;
                        break;
                    case Sub:
                        cpsl_log->out << "\tsub " << reg->name() << ", " << reg->name() << ", " << rhs_str << endl;
                        break;
                    case Mul:
                        cpsl_log->out << "\tmul " << reg->name() << ", " << reg->name() << ", " << rhs_str << endl;
                        break;
                    case Div:
                        cpsl_log->out << "\tdiv " << reg->name() << ", " << reg->name() << ", " << rhs_str << endl;
                        break;
                    case Mod:
                        cpsl_log->out << "\tdiv " << reg->name() << ", " << reg->name() << ", " << rhs_str << endl;
                        cpsl_log->out << "\tmfhi " << reg->name() << " # get quotient of div" << endl;
                        break;
                    case Eq:
                        cpsl_log->out << "\tseq " << reg->name() << ", " << reg->name() << ", " << rhs_str << endl;
                        break;
                    case Ne:
                        cpsl_log->out << "\tsne " << reg->name() << ", " << reg->name() << ", " << rhs_str << endl;
                        break;
                    case Lt:
                        cpsl_log->out << "\tslt " << reg->name() << ", " << reg->name() << ", " << rhs_str << endl;
                        break;
                    case Gt:
                        cpsl_log->out << "\tsgt " << reg->name() << ", " << reg->name() << ", " << rhs_str << endl;
                        break;
                    case Lte:
                        cpsl_log->out << "\tsle " << reg->name() << ", " << reg->name() << ", " << rhs_str << endl;
                        break;
                    case Gte:
                        cpsl_log->out << "\tsge " << reg->name() << ", " << reg->name() << ", " << rhs_str << endl;
                        break;
                    case Bar:
                        cpsl_log->out << "\tor " << reg->name() << ", " << reg->name() << ", " << rhs_str << endl;
                        break;
                    case Amp:
                        cpsl_log->out << "\tand " << reg->name() << ", " << reg->name() << ", " << rhs_str << endl;
                        break;
                    default:
                        {
                            cerr << "Incorrect binary operator " << toString() << " " << toString(op) << " on line " << yylineno << endl;
                            exit(1);
                        }
                }

                if(e->reg)
                    e->free();

                Expression *exp = new Expression(this);
                exp->setType(op, false); //not const since we couldn't fold

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

    //Should never get here
    return this;
}

Expression* Expression::exec(Operation op)
{
    if(bison_verbose)
        cout << "exec(" << toString(op) << ") for expr " << toString() << " on line " << yylineno << endl;

    switch(op)
    {
        case Pred:
            {
                Expression *e = new Expression(new Symbol(1));
                Expression *e2 = new Expression(this);
                return e2->exec(e, Sub);
            }
            break;
        case Succ:
            {
                Expression *e = new Expression(new Symbol(1));
                Expression *e2 = new Expression(this);
                return e2->exec(e, Add);
            }
            break;
        case Negate:
            {
                Expression *e = new Expression(this);

                if(Type::isConst(symbol->type->vt))
                {
                    switch(symbol->type->vt)
                    {
                        case Type::Const_Integer:
                            e->symbol->int_value = -symbol->int_value;
                            break;
                        case Type::Const_Bool:
                            e->symbol->bool_value = -symbol->bool_value;
                            break;
                        default:
                            invalidType(op);
                            break;
                    }
                    return e;
                }
                else
                {
                    loadInTemp();
                    cpsl_log->out << "\tneg " << reg->name() << ", " << reg->name() << " #negate " << symbol->name << " on line: " << yylineno << endl;
                }
            }
            break;
        case Chr:
        case Ord:
            {
                Expression *e = new Expression(this);
                e->setType(op);
                return e;
            }
            break;
        case Tilde:
            {
                Expression *e = new Expression(this);
                if(Type::isConst(symbol->type->vt))
                {
                    switch(symbol->type->vt)
                    {
                        case Type::Const_Integer:
                            e->symbol->int_value = !symbol->int_value;
                            break;
                        case Type::Const_Bool:
                            e->symbol->bool_value = !symbol->bool_value;
                            break;
                        default:
                            invalidType(op);
                            break;
                    }
                }
                else
                {
                    loadInTemp();
                    cpsl_log->out << "\tnot " << reg->name() << ", " << reg->name() << " #bitwise NOT " << symbol->name << " on line: " << yylineno << endl;
                }
            }
            break;
        default:
            {
                cerr << "Unsupported unary expression operator " << toString(op) << " " << toString() << " on line: " << yylineno << endl;
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
    return Type::isConst(symbol->type->vt) && Type::isConst(e->symbol->type->vt) &&
           Type::isFoldable(symbol->type->vt) && Type::isFoldable(e->symbol->type->vt);
}

bool Expression::canFold(Operation op)
{
    switch(op)
    {
        case Add:
        case Sub:
        case Mul:
        case Div:
        case Mod:
            return true;
        default:
            return false;
            break;
    }
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

    switch(symbol->type->vt)
    {
        case Type::Const_Integer:
            {
                cpsl_log->out << "\tli $v0, 1" << endl;
                cpsl_log->out << "\tli $a0, " << symbol->int_value << " #" << toString() << " line: " << yylineno << endl;
            }
            break;
        case Type::Const_String:
        case Type::Const_Char:
            {
                cpsl_log->out << "\tli $v0, 4" << endl;
                cpsl_log->out << "\tla $a0, " << symbol->name << " # " << symbol->str_value << " line: " << yylineno << endl;
            }
            break;
        case Type::Integer:
            {
                cpsl_log->out << "\tli $v0 1" << endl;
                if(reg)
                {
                    cpsl_log->out << "\tadd $a0, " << reg->name() << ", 0 # Set printing register for symbol " << symbol->toString() << endl;
                }
                else
                {
                    cpsl_log->out << "\tlw $a0, " << symbol->offset << "(" << symbol->reg() << ") # Load " << toString() << endl;
                }
            }
            break;
        case Type::Bool:
            {
                cpsl_log->out << "\tli $v0 1" << endl;
                if(reg)
                {
                    cpsl_log->out << "\tsne $a0, " << reg->name() << ", 0 # Set printing register for symbol " << symbol->toString() << " on line " << yylineno << endl;
                }
                else
                {
                    cpsl_log->out << "\tlb $a0, " << symbol->offset << "(" << symbol->reg() << ") # load " << toString() << " on line " << yylineno << endl;
                    cpsl_log->out << "\tsne $a0, $a0, 0 # Boolean var set only to 1 or 0" << endl;
                }
            }
            break;
        case Type::Char:
            {
                cpsl_log->out << "\tli $v0 11" << endl; //print character
                if(reg)
                {
                    cpsl_log->out << "\tadd $a0, " << reg->name() << ", 0 # Set printing register for symbol " << symbol->toString() << endl;
                }
                else
                {
                    cpsl_log->out << "\tlw $a0, " << symbol->offset << "(" << symbol->reg() << ") # Load " << toString() << " on line " << yylineno << endl;
                }
            }
            break;
        case Type::Const_Bool:
            {
                cpsl_log->out << "\tli $v0, 1" << endl;
                cpsl_log->out << "\tli $a0, " << symbol->bool_value << " #" << toString() << " line: " << yylineno << endl;
            }
            break;
        case Type::Unknown:
            {
                //skip unknown for now
                if(bison_verbose)
                {
                    cerr << "unknown type found " << toString() << " on line " << yylineno << endl;
                    exit(1);
                }
            }
            break;
        case Type::Array:
        case Type::Record:
            break;
        default:
            {
                cerr << "Unimplemented print type " << toString() << " on line: " << yylineno << endl;
                exit(1);
            }
        break;
            break;
    }

    cpsl_log->out << "\tsyscall" << endl;
    free();
}

void Expression::loadInTemp()
{
    if(reg)//already done!
        return;

    reg = Register::FindRegister(Register::Temp);

    switch(symbol->type->vt)
    {
        case Type::Integer:
            {
                cpsl_log->out << "\tlw " << reg->name() << ", " << symbol->offset << "(" << symbol->reg() << ") "
                    << "#Loading symbol " << symbol->name << " into reg " << reg->name() << " on line: " << yylineno << endl;
            }
            break;
        case Type::Bool:
        case Type::Char:
            {
                cpsl_log->out << "\tlb " << reg->name() << ", " << symbol->offset << "(" << symbol->reg() << ") "
                    << "#Loading symbol " << symbol->name << " into reg " << reg->name() << " on line: " << yylineno << endl;
            }
            break;
        case Type::Const_Integer:
            {
                cpsl_log->out << "\tli " << reg->name() << ", " << symbol->int_value
                    << "#Loading const integer into reg " << reg->name() << " on line: " << yylineno << endl;
            }
            break;
        case Type::Const_Bool:
            {
                cpsl_log->out << "\tli " << reg->name() << ", " << symbol->bool_value
                    << "#Loading const bool into reg " << reg->name() << " on line: " << yylineno << endl;
            }
            break;
        case Type::Const_String:
            {
                cpsl_log->out << "\tla " << reg->name() << ", " << symbol->name
                    << " # Loading " << toString() << " on line: " << yylineno << endl;
            }
            break;
        case Type::Const_Char:
            {
                cpsl_log->out << "\tli " << reg->name() << ", " << (int)symbol->char_value
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

void Expression::store(int offset, std::string regstr)
{
    loadInTemp();

    if(offset == -1)
        offset = symbol->offset;

    if(regstr.empty())
        regstr = symbol->reg();

    switch(symbol->type->vt)
    {
        case Type::Bool:
        case Type::Char:
        {
            cpsl_log->out << "\tsb " << reg->name() << ", " << offset << "(" << regstr << ") #storing var (" << symbol->toString() << ") on line: " << yylineno << endl;
        }
        break;
        default:
        {
            cpsl_log->out << "\tsw " << reg->name() << ", " << offset << "(" << regstr << ") #storing var (" << symbol->toString() << ") on line: " << yylineno << endl;
        }
    }

    free();
}

void Expression::assign(Symbol* s)
{
    if(!Type::match(symbol->type->vt, s->type->vt))
    {
        cerr << "Expression type mismatch: "
            << toString() << "-"
            << s->toString() << " on line " << yylineno << endl;
        exit(1);
    }

    //s is actually the lhs
    if(Type::isConst(s->type->vt))
    {
        if(bison_verbose)
        {
            cerr << "const lValue (" << toString() << ") assignment not allowed on line " << yylineno << endl;
            exit(1);
        }
        return;
    }

    loadInTemp();
    switch(s->type->vt)
    {
        case Type::Bool:
        case Type::Char:
            {
                cpsl_log->out << "\tsb " << reg->name() << ", " << s->offset << "(" << s->reg() << ") #Assign var (" << s->toString() << ") to (" << toString() << ") on line: " << yylineno << endl;
            }
            break;
        default:
            {
                cpsl_log->out << "\tsw " << reg->name() << ", " << s->offset << "(" << s->reg() << ") #Assign var (" << s->toString() << ") to (" << toString() << ") on line: " << yylineno << endl;
            }
        break;
    }

    symbol = new Symbol(s);

    if(bison_verbose)
        cout << "assigned" << toString() << endl;

    Register::ReleaseRegister(reg);
    reg = NULL;
}

void Expression::setType(Operation op, bool isConst)
{
    if(bison_verbose)
        cpsl_log->out << "setting type for expr " << toString() << " with oper " << toString(op) << " on line " << yylineno << endl;

    Type::ValueType t = Type::Unknown;
    switch(op)
    {
        case Add:
        case Sub:
        case Mul:
        case Div:
        case Mod:
        case Ord:
            t = Type::Integer;
            break;
        case Eq:
        case Ne:
        case Lt:
        case Gt:
        case Lte:
        case Gte:
        case Bar:
        case Amp:
        case Tilde:
            t = Type::Bool;
            break;
        case Chr:
            t = Type::Char;
            break;
        default:
            break;
    }

    if(Type::isConst(symbol->type->vt) && isConst)
        t = Type::const_val(t);

    symbol->setType(t);

    if(bison_verbose)
        cout << "after setting type for expr " << toString() << " with oper " << toString(op) << " on line " << yylineno << endl;
}

void Expression::setVal(int v)
{
    if(bison_verbose)
        cout << "setting value " << v << " for " << toString() << " on line " << yylineno << endl;

    switch(symbol->type->vt)
    {
        case Type::Const_Integer:
            symbol->int_value = v;
            break;
        case Type::Const_Bool:
            symbol->bool_value = v == 0 ? false : true;
            break;
        case Type::Const_Char:
            symbol->char_value = (char)v;
            break;
        default:
            {
                cerr << "Attempting to set incorrect or unimplemented const value " << toString() << " on line " << yylineno << endl;
                exit(1);
            }
    }
}

Type::ValueType Expression::type()
{ 
    return symbol->type->vt;
}
