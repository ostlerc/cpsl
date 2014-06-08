#ifndef __SYMBOL_TABLE_H__
#define __SYMBOL_TABLE_H__

#include <string>
#include <map>
#include <vector>
#include <stack>

#include "Symbol.h"
#include "Type.h"
#include "Expression.h"

class SymbolTableLevel;
struct Parameters;

typedef ListWrapper<std::string> StrList;
typedef ListWrapper<Parameters> ParamList;

class SymbolTable
{
    //Private constructor
    SymbolTable();
    virtual ~SymbolTable();
    public:
    static SymbolTable* instance();

    Expression* expression(int);
    Expression* expression(std::string*);
    Expression* expression_string(std::string*);
    Expression* expression_char(std::string*);
    Expression* lValue(Symbol* s);
    Symbol* findSymbol(std::string*, bool err = true);
    Symbol* findSymbol(std::string, bool err = true);
    void declare_const(std::string *name, Expression* e);
    void create_vars(std::string *type_string, std::vector<std::string> var_list);
    void read(std::vector<Symbol*> sym_list);
    void print(std::vector<Expression*> expr_list);
    void begin();
    void initialize();
    void end();
    void checkRegisters();
    void enterScope();
    void exitScope();
    Expression* assign(Symbol* s, Expression* e);
    Expression* assign(std::string s, Expression* e);

    void push(std::string reg);
    void pop(std::string reg);
    void set(std::string lhs, std::string rhs);

    //while
    std::string* whileStart();
    std::string* whileExpr(Expression* e);
    void whileStatement(std::string* startLbl, std::string *endLbl);
    void stop();

    //if
    std::string* ifExpr(Expression* e);
    void ifCondition(std::string* endLbl);
    void ifStatement();
    std::string* elseifExpr(Expression *e);
    void elseifStatement(std::string* lbl);

    //for
    std::string* forExpr(Expression* lhs, Expression* rhs, Expression::Operation op);
    void forStatement(std::string* lbl);

    //repeat
    std::string* repeatHead();
    void repeatStatement(std::string* lbl, Expression *e);

    //hackery for now
    Expression* unimp();
    void ignoreNextLValue();

    Symbol* forwardProc(std::string* id, std::vector<Parameters> params);
    void procedureHead();
    void procedureParams(std::string* id, std::vector<Parameters> params);
    void endProcedure();
    void callProc(std::string* proc, std::vector<Expression*> expr_list = {});

    std::string paramsString(Parameters params);
    std::string paramsString(std::vector<Expression*>& exprs);
    std::string procId(std::string id, Parameters params);
    std::string procId(std::string id, std::vector<Parameters>& params);
    std::string procId(std::string id, std::vector<Expression*> exprs);

    private:

    std::vector<Symbol*> c_symbols; //used to write out const string data at end of program, there is no scope for const literals
    std::vector<SymbolTableLevel*> levels;

    std::map<std::string, std::stack<std::string>> lbl_stack;
    bool ignore_next_lval; //TODO: remove when types are done
};


struct Parameters
{
    Parameters(std::vector<std::string> sl, Type::ValueType type) : vars(sl) , type(type) {}
    std::vector<std::string> vars;
    Type::ValueType type;
};

#endif //__SYMBOL_TABLE_H__
