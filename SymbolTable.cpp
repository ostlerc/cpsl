#include "SymbolTable.h"
#include "SymbolTableLevel.h"
#include "Expression.h"
#include "Type.h"
#include "Log.h"

#include <iostream>
#include <algorithm>

using namespace std;

extern int yylineno; // defined and maintained in lex.cpp
extern bool bison_verbose;
extern Log* cpsl_log;

SymbolTable::SymbolTable()
    : ignore_next_lval(false)
{
    levels.emplace_back(new SymbolTableLevel(true));
    initialize();
}

SymbolTable::~SymbolTable()
{ }

SymbolTable* SymbolTable::instance()
{
    static SymbolTable* st = NULL;
    if(!st)
        st = new SymbolTable;

    return st;
}

void SymbolTable::declare_const(std::string *name, Expression *exp)
{
    levels.back()->addVariable(*name, exp->symbol->type);
}

Expression* SymbolTable::expression(int i)
{
    if(bison_verbose)
        cout << "expression int " << i << endl;
    return new Expression(new Symbol(i));
}

Expression* SymbolTable::unimp()
{
    return new Expression(new Symbol(1)); //TODO: this is so we don't get errors.
    //cerr << "Unimplemented function " << __FUNCTION__ << " line: " << yylineno << endl;
    //exit(1);
}

Expression* SymbolTable::lValue(Symbol* s)
{
    Expression *e = new Expression(s);
    return e;
}

Expression* SymbolTable::expression_string(string* s)
{
    if(bison_verbose)
        cout << "expression_string '" << *s << "'" << endl;

    for(unsigned int i = 0; i < c_symbols.size(); i++)
    {
        if(c_symbols[i]->str_value == *s && c_symbols[i]->type == Type::Const_String)
            return new Expression(c_symbols[i]);
    }

    Symbol *sym = new Symbol(*s);
    c_symbols.push_back(sym);
    return new Expression(sym);
}

Expression* SymbolTable::expression_char(string* s)
{
    if(bison_verbose)
        cout << "expression_char '" << *s << "'" << endl;

    //remove character ticks and add quotes for the expression/symbol name
    auto it = remove_if(std::begin(*s), std::end(*s), [](char c) {return (c == '\'');});
    s->erase(it, std::end(*s));

    *s = "\"" + *s + "\"";

    char c = s->at(1);

    if((*s)[1] == '\\')
    {
        switch((*s)[2])
        {
            case 'n':
                c = '\n';
                break;
            case 'r':
                c = '\r';
                break;
            case 'b':
                c = '\b';
                break;
            case 'f':
                c = '\f';
                break;
            case 't':
                c = '\t';
                break;
            default:
                {
                    if(bison_verbose)
                        cout << "special char used other than regular ones..." << endl;

                    c = (*s)[2];
                }
        }
    }

    for(unsigned int i = 0; i < c_symbols.size(); i++)
    {
        if(c_symbols[i]->char_value == c && c_symbols[i]->type == Type::Const_Char)
            return new Expression(c_symbols[i]);
    }

    Symbol *sym = new Symbol(c, *s);
    c_symbols.push_back(sym);
    return new Expression(sym);
}

Symbol* SymbolTable::findSymbol(string s, bool err)
{
    return findSymbol(&s, err);
}

Symbol* SymbolTable::findSymbol(string* s, bool err)
{
    auto res = levels.back()->lookupVariable(*s);
    if(res) return res;

    res = levels.front()->lookupVariable(*s); //front is global
    if(res) return res;

    if(!err)
        return NULL;

    cerr << "could not find symbol " << *s << " line: " << yylineno << endl;
    exit(1);
}

void SymbolTable::create_vars(std::string *type_string, vector<std::string> var_list)
{
    Type::ValueType type = Type::fromString(*type_string, false);

    for(int i = var_list.size()-1; i >= 0; i--)
    {
        levels.back()->addVariable(var_list[i], type);
    }
}

void SymbolTable::print(vector<Expression*> expr_list)
{
    if(expr_list.size() == 0)
    {
        cerr << "Missing arguments on line: " << yylineno << endl;
        exit(1);
    }

    for(unsigned int i = 0; i < expr_list.size(); i++)
        expr_list[i]->print();

    checkRegisters();
}

void SymbolTable::checkRegisters()
{
    if(Register::reservedRegisters() > 0)
    {
        cerr << "failed to release all registers! line: " << yylineno << endl;
        exit(1);
    }
}

void SymbolTable::read(vector<Symbol*> sym_list)
{
    if(sym_list.size() == 0)
    {
        cerr << "Missing arguments on line: " << yylineno << endl;
        exit(1);
    }

    for(unsigned int i = 0; i < sym_list.size(); i++)
        sym_list[i]->read();

    checkRegisters();
}

void SymbolTable::begin()
{
    cpsl_log->out << "\t.text" << endl;
    cpsl_log->out << "main:\n";
    enterScope();
}

void SymbolTable::initialize()
{
    levels.front()->addVariable("true", Type::Const_Bool);
    levels.front()->addVariable("TRUE", Type::Const_Bool);
    levels.front()->addVariable("false", Type::Const_Bool);
    levels.front()->addVariable("FALSE", Type::Const_Bool);

    auto v = levels.front()->lookupVariable("true"); v->bool_value = true;
         v = levels.front()->lookupVariable("TRUE"); v->bool_value = true;
         v = levels.front()->lookupVariable("false"); v->bool_value = false;
         v = levels.front()->lookupVariable("FALSE"); v->bool_value = false;

     //check declared procedures all exist
     levels.front()->checkProcedures();
}

void SymbolTable::end()
{
    cpsl_log->out << "\tli $v0, 10" << endl;
    cpsl_log->out << "\tsyscall" << endl;

    if(c_symbols.size() > 0)
    {
        cpsl_log->out << "\t.data" << endl;

        if(bison_verbose)
            cout << "there are " << c_symbols.size() << " constant entries to add" << endl;

        for(unsigned int i = 0; i < c_symbols.size(); i++)
            c_symbols[i]->store();
    }

    exitScope();
}

Expression* SymbolTable::assign(std::string s, Expression* e)
{
    return assign(findSymbol(s), e);
}

Expression* SymbolTable::assign(Symbol* s, Expression* e)
{
    if(ignore_next_lval)
        ignore_next_lval = false;
    else
        e->assign(s);

    return e;
}

void SymbolTable::ignoreNextLValue()
{
    ignore_next_lval = true;
}

string* SymbolTable::whileStart()
{
    string *startLbl = new string(Symbol::GetLabel("WS"));
    cpsl_log->out <<  *startLbl << ":" << endl;
    return startLbl;
}

string* SymbolTable::whileExpr(Expression* e)
{
    e->loadInTemp();
    string *endLbl = new string(Symbol::GetLabel("WE"));
    cpsl_log->out << "\tbeq " << e->reg->name() << ", $zero, " << *endLbl << ".stop # exit 'while' branch expr: " << e->toString() << " on line " << yylineno << endl;
    lbl_stack["stop"].push(*endLbl);

    e->free();

    return endLbl;
}

void SymbolTable::whileStatement(std::string* startLbl, std::string *endLbl)
{
    cpsl_log->out << "\tj " << *startLbl << " # end while on line " << yylineno << endl;
    cpsl_log->out << *endLbl << ".stop:" << endl;
    lbl_stack["stop"].pop();
}

string* SymbolTable::ifExpr(Expression* e)
{
    string *endLbl = new string(Symbol::GetLabel("IfE"));
    cpsl_log->out << "\tbeq " << e->reg->name() << ", $zero, " << *endLbl << " # exit 'if' branch expr: " << e->toString() << " on line " << yylineno << endl;
    e->free();

    return endLbl;
}

void SymbolTable::ifCondition(std::string* endLbl)
{
    string endifLbl = Symbol::GetLabel("EIf");
    cpsl_log->out << "\tj " << endifLbl << " #end 'if' on line " << yylineno << endl;
    lbl_stack["if"].push(endifLbl);

    cpsl_log->out << *endLbl << ":" << endl;
}

void SymbolTable::ifStatement()
{
    string lbl = lbl_stack["if"].top();
    lbl_stack["if"].pop();
    cpsl_log->out << lbl << ":" << " # end of 'if' statement on line " << yylineno << endl;
}

string* SymbolTable::elseifExpr(Expression *e)
{
    string *lbl = new string(Symbol::GetLabel("IfE"));
    cpsl_log->out << "\tbeq " << e->reg->name() << ", $zero, " << *lbl << " # exit 'elseif' branch expr: " << e->toString() << " on line " << yylineno << endl;
    e->free();
    return lbl;
}

void SymbolTable::elseifStatement(std::string* lbl)
{
    cpsl_log->out << "\tj " << lbl_stack["if"].top() << endl;
    cpsl_log->out << *lbl << ": " << " # end 'elseif' from line: " << yylineno << endl;
}

void SymbolTable::stop()
{
    cpsl_log->out << "\tli $v0, 10 # stopping on line " << yylineno << endl;
    cpsl_log->out << "\tsyscall" << endl;
}

string* SymbolTable::forExpr(Expression* lhs, Expression* rhs, Expression::Operation op)
{
    string *lbl = new string(Symbol::GetLabel("For"));
    cpsl_log->out << "\tj " << *lbl << ".begin " << endl;
    cpsl_log->out << *lbl << ".start:" << endl;

    Expression *t = lhs->exec(op);
    t->store();

    cpsl_log->out << *lbl << ".begin:" << endl;
    Expression *lt = rhs->exec(lhs, op == Expression::Succ ? Expression::Gte : Expression::Lte);
    cpsl_log->out << "\tbeq " << lt->reg->name() << ", $zero " << *lbl << ".stop # line " << yylineno << endl;
    lbl_stack["stop"].push(*lbl);
    lt->free();

    lhs->free();
    rhs->free();
    t->free();

    return lbl;
}

void SymbolTable::forStatement(string* lbl)
{
    cpsl_log->out << "\tj " << *lbl << ".start" << endl;
    cpsl_log->out << *lbl << ".stop: # end for on line " << yylineno << std::endl;
    lbl_stack["stop"].pop();
}

std::string* SymbolTable::repeatHead()
{
    string *lbl = new string(Symbol::GetLabel("RS"));
    cpsl_log->out <<  *lbl << ":" << endl;
    lbl_stack["stop"].push(*lbl);
    return lbl;
}

void SymbolTable::repeatStatement(std::string* lbl, Expression *e)
{
    e->loadInTemp();
    cpsl_log->out << "\tbeq " << e->reg->name() << ", $zero " << *lbl << "# end repeat statemnt on line " << yylineno << endl;
    cpsl_log->out << *lbl << ".stop:" << endl;
    lbl_stack["stop"].pop();
    e->free();
}

void SymbolTable::procedureParams(string id, std::vector<Parameters> params, std::string type)
{
    enterScope();
    cpsl_log->out << endl << "######################" << endl;

    lbl_stack["return_type"].push(type);

    Symbol *sym = findSymbol(procId(id, params), false);
    if(!sym)
    {
        if(type.empty())
            sym = forwardProc(id, params);
        else
            sym = forwardFunc(id, params, type);
    }
    else if(sym->type != Type::Procedure && sym->type != Type::Function)
    {
        cerr << "Symbol is not of type function/procedure: " << id << " on line " << yylineno << endl;
        exit(1);
    }
    else if(sym->bool_value == true)
    {
        cerr << "Redeclaration of " << id << " on line " << yylineno << endl;
        exit(1);
    }
    else if(sym->type == Type::Function)
    {
        if(sym->returnType != Type::fromString(type))
        {
            cerr << "incorrect return types for function " << id << ". Expecting type " << Type::toString(sym->returnType) << " on line " << yylineno << endl;
            exit(1);
        }
    }

    //I am using this bool_value as a flag if the procedure has been declared or not
    sym->bool_value = true;

    cpsl_log->out << "\t.text" << endl;
    cpsl_log->out << "proc." << procId(id, params) << ":";
    lbl_stack["proc_lbl"].push(procId(id, params));
    if(bison_verbose)
        cout << " #declaring procedure on line " << yylineno;
    cpsl_log->out << endl;

    cpsl_log->out << "#callee procpsl_logue" << endl;
    levels.back()->loadParams(params);
}

void SymbolTable::endProcedure(std::vector<Parameters> params)
{
    cpsl_log->out << "proc." << lbl_stack["proc_lbl"].top() << ".end:" << endl;
    levels.back()->unloadParams(params);
    cpsl_log->out << "\tjr $ra" << endl;
    cpsl_log->out << "######################" << endl << endl;
    exitScope();
    lbl_stack["return_type"].pop();
    lbl_stack["proc_lbl"].pop();
}

void SymbolTable::_return(Expression *exp)
{
    std::string type = lbl_stack["return_type"].top();
    if(!!exp == type.empty())
    {
        cerr << "Missing or extra type on line " << yylineno << endl;
        exit(1);
    }

    if(exp)
    {
        if(bison_verbose)
            cout << "comparing" << type << Type::toString(exp->symbol->type) << " on line " << yylineno << endl;
        if(Type::fromString(type) != Type::nonconst_val(exp->symbol->type))
        {
            cerr << "Incorrect type on line " << yylineno << endl;
            cerr << "expecting type " << Type::toString(exp->symbol->type) << endl;
            exit(1);
        }

        exp->loadInTemp();
        cpsl_log->out << "\tadd $v0, " << exp->reg->name() << ", $zero" << endl;
        exp->free();
    }
    cpsl_log->out << "\tj proc." << lbl_stack["proc_lbl"].top() << ".end" << endl;
}

void SymbolTable::enterScope()
{
    levels.push_back(new SymbolTableLevel(false));
}

void SymbolTable::exitScope()
{
    levels.pop_back();
}

void SymbolTable::callProc(std::string proc, vector<Expression*> expr_list)
{
    procBoiler(proc, expr_list, Type::Procedure);
}

Symbol* SymbolTable::procBoiler(std::string proc, vector<Expression*> expr_list, Type::ValueType fType)
{
    std::string lbl = procId(proc, expr_list);
    Symbol *s = findSymbol(lbl);

    if(s->type != fType)
    {
        if(s->type == Type::Function)
            cerr << "function '" << proc << "' used as a procedure on line " << yylineno << endl;
        else
            cerr << "variable '" << proc << "' is not of type '" << Type::toString(fType) << "' on line " << yylineno << endl;
        exit(1);
    }

    cpsl_log->out << "#caller procpsl_logue" << endl;
    push("$ra");
    push("$fp");
    Register *tmp = Register::FindRegister(Register::Temp);
    set(tmp->name(), "$sp");
    levels.back()->saveExpressions(expr_list);
    set("$fp",tmp->name());
    Register::ReleaseRegister(tmp);
    cpsl_log->out << "\tjal proc." << lbl << " # calling procedure " << proc << " on line " << yylineno << endl;
    cpsl_log->out << "#caller epicpsl_logue" << endl;
    set("$sp","$fp");
    pop("$fp");
    pop("$ra");

    return s;
}

Expression* SymbolTable::callFunc(std::string func, vector<Expression*> expr_list)
{
    Symbol *s = procBoiler(func, expr_list, Type::Function);

    if(bison_verbose)
        cout << "return type for function " << func << " is " << Type::toString(s->returnType) << " on line " << yylineno << endl;
    //Assign return value to expression
    Symbol *ret_sym = levels.back()->addVariable(Symbol::GetLabel("ret"), s->returnType, false);
    Expression *ret = new Expression(ret_sym);
    Register *tmp = Register::FindRegister(Register::Temp);
    set(tmp->name(), "$v0");
    ret->reg = tmp;
    ret->store(-1, "$fp");
    return ret;
}


Symbol* SymbolTable::forwardProc(std::string id, std::vector<Parameters> params)
{
    Symbol *s = findSymbol(procId(id, params), false);
    if(s)
    {
        cerr << "Re-forward declaration of procedure " << id << " on line " << yylineno << endl;
        exit(1);
    }

    s = levels.front()->addProcedure(procId(id, params));
    return s;
}

Symbol* SymbolTable::forwardFunc(std::string id, std::vector<Parameters> params, std::string type)
{
    Symbol *s = findSymbol(procId(id, params), false);
    if(s)
    {
        cerr << "Re-forward declaration of function " << id << " on line " << yylineno << endl;
        exit(1);
    }

    s = levels.front()->addFunction(procId(id, params), Type::fromString(type));
    return s;
}

void SymbolTable::push(std::string reg)
{
    cpsl_log->out << "\tsw " << reg << ", 0($sp) #push onto stack" << endl;
    cpsl_log->out << "\tadd $sp, $sp, -4" << endl;
}

void SymbolTable::pop(std::string reg)
{
    cpsl_log->out << "\tadd $sp, $sp, 4" << endl;
    cpsl_log->out << "\tlw " << reg << ", 0($sp) #pop off stack" << endl;
}

void SymbolTable::set(std::string lhs, std::string rhs)
{
    cpsl_log->out << "\tadd " << lhs << ", " << rhs << ", $zero" << endl;
}

std::string SymbolTable::paramsString(Parameters params)
{
    std::string ret;
    std::string tstr = Type::toString(params.type);
    for(unsigned int i = 0; i < params.vars.size(); i++)
    {
        if(i)
            ret += ".";

        ret += tstr;
    }

    return ret;
}

std::string SymbolTable::paramsString(std::vector<Expression*>& exprs)
{
    std::string ret;
    bool first = true;
    for(auto&e : exprs)
    {
        if(!first)
            ret += ".";
        else
            first = false;
        std::string tstr = Type::toString(Type::nonconst_val(e->type()));
        ret += tstr;
    }

    return ret;
}

std::string SymbolTable::procId(std::string id, Parameters params)
{
    if(params.vars.size() > 0)
        return id + "_" + paramsString(params);

    return id;
}

std::string SymbolTable::procId(std::string id, std::vector<Parameters>& params)
{
    std::string ret = id;
    if(params.size() > 0)
    {
        ret += "_";
        bool first = true;
        for(auto v : params)
        {
            if(first)
                ret += paramsString(v);
            else
                ret += "," + paramsString(v);
        }
    }

    return ret;
}

std::string SymbolTable::procId(std::string id, vector<Expression*> exprs)
{
    if(exprs.size() > 0)
        return id + "_" + paramsString(exprs);

    return id;
}
