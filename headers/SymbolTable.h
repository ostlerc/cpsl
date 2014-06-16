#ifndef __SYMBOL_TABLE_H__
#define __SYMBOL_TABLE_H__

#include <string>
#include <map>
#include <vector>
#include <stack>

#include "Type.h"
#include "Expression.h"

class Symbol;
class SymbolTableLevel;
struct Parameters;
struct RecordEntry;

typedef ListWrapper<std::string> StrList;
typedef ListWrapper<Parameters> ParamList;
typedef ListWrapper<RecordEntry> RecordList;

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

    Symbol* findSymbol(std::string, bool err = true);
    Type* findType(std::string, bool err = true);

    void startTypeDeclare(std::string name);
    void endType();
    void declare_const(std::string name, Expression* e);
    void create_vars(Type *type, std::vector<std::string> var_list);
    void read(std::vector<Expression*> expr_list);
    void print(std::vector<Expression*> expr_list);
    void begin();
    void initialize();
    void end();
    void checkRegisters();
    void enterScope();
    void exitScope();
    Expression* assign(Expression* lhs, Expression* e);

    void push(std::string reg);
    void pop(std::string reg);
    void set(std::string lhs, std::string rhs);

    //while
    std::string* whileStart();
    std::string* whileExpr(Expression* e);
    void whileStatement(std::string startLbl, std::string endLbl);
    void stop();

    //if
    std::string* ifExpr(Expression* e);
    void ifCondition(std::string endLbl);
    void ifStatement();
    std::string* elseifExpr(Expression *e);
    void elseifStatement(std::string lbl);

    //for
    std::string* forExpr(Expression* lhs, Expression* rhs, Expression::Operation op);
    void forStatement(std::string lbl);

    //repeat
    std::string* repeatHead();
    void repeatStatement(std::string* lbl, Expression *e);

    //hackery for now
    Expression* unimp();

    Symbol* forwardProc(std::string id, std::vector<Parameters> params);
    Symbol* forwardFunc(std::string id, std::vector<Parameters> params, Type *type);
    void procedureParams(std::string id, std::vector<Parameters> params, Type *type = NULL);
    void endProcedure(std::vector<Parameters> params);
    void callProc(std::string proc, std::vector<Expression*> expr_list = {});
    Expression* callFunc(std::string proc, std::vector<Expression*> expr_list = {});
    Symbol* procBoiler(std::string proc, std::vector<Expression*> expr_list, Type* fType);
    void _return(Expression* exp = NULL);

    Type* arrayType(Expression *lhs, Expression *rhs, Type *type);
    Expression* arrayIndex(Expression *parent, Expression *index);
    Type* recordType(std::vector<RecordEntry>& entries);
    Expression* recordMember(Expression* rec, std::string member);
    void setupSymbol(Symbol* s, Expression* p);

    std::string paramsString(Parameters params);
    std::string paramsString(std::vector<Expression*>& exprs);
    std::string procId(std::string id, Parameters params);
    std::string procId(std::string id, std::vector<Parameters>& params);
    std::string procId(std::string id, std::vector<Expression*> exprs);

    Type* typeOf(Type::ValueType vt);

    private:

    std::vector<Symbol*> c_symbols; //used to write out const string data at end of program, there is no scope for const literals
    std::vector<SymbolTableLevel*> levels;

    std::map<std::string, std::stack<std::string>> lbl_stack;
    std::map<std::string, std::stack<Type*>> type_stack;

    Type* cbool;
    Type* cint;
    Type* cchar;
    Type* cstr;
    Type* boolt;
    Type* intt;
    Type* chart;
};


struct Parameters
{
    Parameters(std::vector<std::string> sl, Type *type) : vars(sl) , type(type) {}
    std::vector<std::string> vars;
    Type *type;
};

struct RecordEntry
{
    RecordEntry(std::vector<std::string>& ids, Type* type) : ids(ids), type(type) {}
    std::vector<std::string> ids;
    Type *type;
};

#endif //__SYMBOL_TABLE_H__
