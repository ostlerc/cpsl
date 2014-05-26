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
        default:
              cerr << "Unknown op " << op << " in function " << __FUNCTION__ << " line: " << yylineno << endl;
              exit(1);
    }
}

Expression* Expression::exec(Expression* e, Operation op)
{
    if(bison_verbose)
        cout << "exec on " << toString() << " " << toString(op) << " " << e->toString() << " on line: " << yylineno << endl;

    if(canFold(e))
    {
        if(bison_verbose)
            cout << "const " << toString(op) << " folding " << symbol->int_value<< ", " << e->symbol->int_value << endl;

        switch(op)
        {
            case Add:
                symbol->int_value += e->symbol->int_value;
                break;
            case Sub:
                symbol->int_value -= e->symbol->int_value;
                break;
            case Mul:
                symbol->int_value *= e->symbol->int_value;
                break;
            case Div:
                symbol->int_value /= e->symbol->int_value;
                break;
            case Mod:
                symbol->int_value = symbol->int_value % e->symbol->int_value;
                break;
            case Eq:
                symbol->bool_value = symbol->int_value == e->symbol->int_value;
                break;
            case Ne:
                symbol->bool_value = symbol->int_value != e->symbol->int_value;
                break;
            case Lt:
                symbol->bool_value = symbol->int_value < e->symbol->int_value;
                break;
            case Gt:
                symbol->bool_value = symbol->int_value > e->symbol->int_value;
                break;
            case Lte:
                symbol->bool_value = symbol->int_value <= e->symbol->int_value;
                break;
            case Gte:
                symbol->bool_value = symbol->int_value >= e->symbol->int_value;
                break;
            default:
                {
                    cerr << "unsupported binary operator " << op << " line: " << yylineno << endl;
                    exit(1);
                }
        }
        delete e;
        setType(op);
        return this;
    }
    switch(symbol->type)
    {
        case Type::Integer:
        case Type::Const_Integer:
            {
                loadInTemp();

                string rhs;
                if(!Type::isConst(e->symbol->type))
                {
                    e->loadInTemp();
                    rhs = e->reg->name();
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
                            rhs = to_string(e->symbol->int_value);
                            break;
                        case Eq:
                        case Ne:
                        case Lt:
                        case Gt:
                        case Lte:
                        case Gte:
                            e->loadInTemp();
                            rhs = e->reg->name();
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
                        cout << "\tadd " << reg->name() << ", " << reg->name() << ", " << rhs << endl;
                        break;
                    case Sub:
                        cout << "\tsub " << reg->name() << ", " << reg->name() << ", " << rhs << endl;
                        break;
                    case Mul:
                        cout << "\tmul " << reg->name() << ", " << reg->name() << ", " << rhs << endl;
                        break;
                    case Div:
                        cout << "\tdiv " << reg->name() << ", " << reg->name() << ", " << rhs << endl;
                        break;
                    case Mod:
                        cout << "\tdiv " << reg->name() << ", " << reg->name() << ", " << rhs << endl;
                        cout << "\tmfhi " << reg->name() << " # get quotient of div" << endl;
                        break;

                        //rhs must be reg for conditionals
                    case Eq:
                        cout << "\tseq " << reg->name() << ", " << reg->name() << ", " << rhs << endl;
                        break;
                    case Ne:
                        cout << "\tsne " << reg->name() << ", " << reg->name() << ", " << rhs << endl;
                        break;
                    case Lt:
                        cout << "\tslt " << reg->name() << ", " << reg->name() << ", " << rhs << endl;
                        break;
                    case Gt:
                        cout << "\tsgt " << reg->name() << ", " << reg->name() << ", " << rhs << endl;
                        break;
                    case Lte:
                        cout << "\tsle " << reg->name() << ", " << reg->name() << ", " << rhs << endl;
                        break;
                    case Gte:
                        cout << "\tsge " << reg->name() << ", " << reg->name() << ", " << rhs << endl;
                        break;
                    default:
                        {
                            cerr << "Incorrect binary operator on line " << yylineno << endl;
                            exit(1);
                        }
                }

                if(e->reg)
                    e->free();

                symbol->type = Type::Integer; //non folding

                setType(op);
                return this;
            }
            break;
        default:
            {
                cerr << "Unimplemented or incorret add type " << symbol->type << " " << e->symbol->type << " line: " << yylineno << endl;
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
                if(Type::isConst(symbol->type))
                    invalidType();

                loadInTemp();
                cout << "\taddi " << reg->name() << ", " << reg->name() << ", 1 #increment by one from line:" << yylineno << endl;
                //cout << "\tsw " << reg->name() << ", " << symbol->offset << "($gp)" << endl;
                //Register::ReleaseRegister(reg);
                //reg = NULL;
            }
            break;
        case Succ:
            {
                if(Type::isConst(symbol->type))
                    invalidType();

                loadInTemp();
                cout << "\tsubi " << reg->name() << ", " << reg->name() << ", 1 #decrement by one from line:" << yylineno << endl;
                //cout << "\tsw " << reg->name() << ", " << symbol->offset << "($gp)" << endl;
                //Register::ReleaseRegister(reg);
                //reg = NULL;
            }
            break;
        case Negate:
            {
                if(Type::isConst(symbol->type))
                    invalidType();

                loadInTemp();
                cout << "\tneg " << reg->name() << ", " << reg->name() << " #negate " << symbol->name << " on line: " << yylineno << endl;
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

void Expression::invalidType()
{
    cerr << "invalid type in expression on line: " << yylineno << endl;
    exit(1);
}

bool Expression::canFold(Expression* e)
{
    return symbol->type == Type::Const_Integer && e->symbol->type == Type::Const_Integer;
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
        default: //non const printing
        //case Type::Char:
        //case Type::Const_Char:
        //case Type::Const_Bool:
        {
            //TODO: finish printing here
            cerr << "Unimplemented print type " << Type::toString(symbol->type) << " on line: " << yylineno << endl;
            exit(1);
        }
        break;
            break;
    }

    cout << "\tsyscall" << endl;
    free();
}

void Expression::store()
{
    if(bison_verbose)
        cout << "storing " << toString() << endl;

    if(symbol->type != Type::Const_String && symbol->type != Type::Const_Char)
    {
        cerr << "Error trying to store non const string on line: " << yylineno << endl;
        exit(1);
    }

    if(!symbol)
    {
        cerr << "Error trying to store const string with NULL symbol on line: " << yylineno << endl;
        exit(1);
    }

    cout << symbol->name << ":\t.asciiz" << symbol->str_value << endl;
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
        case Type::Const_Integer:
            {
                cout << "\tli " << reg->name() << ", " << symbol->int_value
                    << "#Loading const integer into reg " << reg->name() << " on line: " << yylineno << endl;
            }
            break;
        case Type::Const_String:
        case Type::Const_Char:
            {
                cout << "\tla " << reg->name() << ", " << symbol->name
                    << "#Loading " << Type::toString(symbol->type) << " (" << symbol->str_value << " into reg " << reg->name() << " on line: " << yylineno << endl;
            }
            break;
        default:
            {
                cerr << "Attempting to load string constant into register invalid or unimplemented on line: " << yylineno << endl;
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
    if(symbol->type != s->type)
    {
        cerr << "Expression type mismatch: " 
            << Type::toString(symbol->type) << ", " 
            << Type::toString(s->type) << " on line " << yylineno << endl;
        exit(1);
    }
    if(Type::isConst(symbol->type))
    {
        cerr << "const lValue assignment not allowed on line " << yylineno << endl;
        exit(1);
    }

    switch(symbol->type)
    {
        case Type::Integer:
            {
                loadInTemp();
                cout << "\tsw " << reg->name() << ", " << s->offset << "($gp) #Assign var " << s->name << " to " << toString() << " on line: " << yylineno << endl;
                Register::ReleaseRegister(reg);
                reg = NULL;
                symbol = s;
            }
            break;
        default:
            {
                cerr << "Attempting to assign var with invalid or unimplemented type on line: " << yylineno << endl;
                exit(1);
            }
    }
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
            symbol->type = Type::Integer;
            break;
        case Eq:
        case Ne:
        case Lt:
        case Gt:
        case Lte:
        case Gte:
            symbol->type = Type::Bool;
            break;
    }
}
