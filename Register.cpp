#include "Register.h"
#include "Log.h"

#include <iostream>

using namespace std;

const int treg_size = 10;
const int sreg_size = 8;

vector<Register*> Register::temp_registers;
vector<Register*> Register::save_registers;

extern int yylineno;// defined and maintained in lex.cpp
extern bool bison_verbose;
extern Log* cpsl_log;

Register::Register(int reg_num, RegisterType type)
    : reg_num(reg_num)
    , allocated(false)
    , type(type)
{}

Register::~Register()
{}

Register* Register::FindRegister(RegisterType type)
{
    //initialize registers
    if(save_registers.size() == 0)
    {
        for(unsigned int i = 0; i < treg_size; i++)
        {
            temp_registers.push_back(new Register(i, Temp));
        }

        for(unsigned int i = 0; i < sreg_size; i++)
        {
            save_registers.push_back(new Register(i, Save));
        }
    }

    switch(type)
    {
        case Temp:
            {
                for(unsigned int i = 0; i < temp_registers.size(); i++)
                {
                    if(!temp_registers[i]->allocated)
                    {
                        temp_registers[i]->allocated = true;
                        if(bison_verbose)
                            cout << "allocated temp register " << i << endl;
                        return temp_registers[i];
                    }
                }
            }
            break;
        case Save:
            {
                for(unsigned int i = 0; i < save_registers.size(); i++)
                {
                    if(!save_registers[i]->allocated)
                    {
                        save_registers[i]->allocated = true;
                        if(bison_verbose)
                            cout << "allocated save register " << i << endl;
                        return save_registers[i];
                    }
                }
            }
            break;
        default:
            cerr << "Unknown register type " << type << " line: " << yylineno << endl;
            exit(1);
    }

    cerr << "Could not allocate register on line: " << yylineno << endl;
    exit(1);
}

string Register::toString()
{
    string o = "register #" + to_string(reg_num) + " type " + ( type == Temp ? "temp" : "save") + " line: ";
    return o;
}

vector<string> Register::allocatedRegisters()
{
    vector<string> registers;
    for(unsigned int i = 0; i < save_registers.size(); i++)
    {
        if(save_registers[i]->allocated)
            registers.push_back("$s" + to_string(i));

        if(temp_registers[i]->allocated)
            registers.push_back("$t" + to_string(i));
    }
    return registers;
}

void Register::ReleaseRegister(Register* reg)
{
    if(bison_verbose)
        cout << "releasing register " << reg->toString() << endl;

    if(!reg) { cerr << "attempt to release NULL register line: " << yylineno << endl; exit(1); }
    Register *r = NULL;

    switch(reg->type)
    {
        case Temp:
            {
                if(reg->reg_num < 0 || reg->reg_num > treg_size) { cerr << "attempt to access invalid index " << reg->reg_num << " line: " << yylineno << endl; exit(1); }
                r = temp_registers[reg->reg_num];
            }
            break;
        case Save:
            {
                if(reg->reg_num < 0 || reg->reg_num > sreg_size) { cerr << "attempt to access invalid index " << reg->reg_num << " line: " << yylineno << endl; exit(1); }
                r = save_registers[reg->reg_num];
            }
            break;
        default:
            cerr << "Unknown register type " << reg->type << " line: " << yylineno << endl;
            exit(1);
    }

    if(!r) { cerr << "failed to find register " << reg->reg_num << " " << reg->type << " line: " << yylineno << endl; exit(1); }

    if(!r->allocated)
    {
        cerr << "double release of register " << reg->reg_num << " type " << reg->type << " line: " << yylineno << endl;
        exit(1);
    }

    //Successful release
    r->allocated = false;
}

string Register::name()
{
    string n = type == Temp ? "$t" : "$s";
    n += to_string(reg_num);
    return n;
}

int Register::reservedRegisters()
{
    int ret = 0;
    for(unsigned int i = 0; i < save_registers.size(); i++)
    {
        if(save_registers[i]->allocated)
            ret++;

        if(temp_registers[i]->allocated)
        {
            if(bison_verbose)
                cerr << "temp register " << i << " not released" << endl;
            ret++;
        }
    }

    return ret;
}
