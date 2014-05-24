#include "Register.h"

#include <iostream>

using namespace std;

const int treg_size = 10;
const int sreg_size = 8;

vector<Register*> Register::temp_registers;
vector<Register*> Register::save_registers;

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
                        cout << "allocated save register " << i << endl;
                        return save_registers[i];
                    }
                }
            }
            break;
        default:
            cerr << "Unknown register type " << type << endl;
            exit(1);
    }
    return NULL;
}

void Register::ReleaseRegister(Register* reg)
{
    Register *r = NULL;
    if(!reg) { cerr << "attempt to release NULL register"; exit(1); }

    switch(reg->type)
    {
        case Temp:
            {
                if(reg->reg_num < 0 || reg->reg_num > treg_size) { cerr << "attempt to access invalid index " << reg->reg_num; exit(1); }
                r = temp_registers[reg->reg_num];
            }
            break;
        case Save:
            {
                if(reg->reg_num < 0 || reg->reg_num > sreg_size) { cerr << "attempt to access invalid index " << reg->reg_num; exit(1); }
                r = save_registers[reg->reg_num];
            }
            break;
        default:
            cerr << "Unknown register type " << reg->type << endl;
            exit(1);
    }

    if(!r) { cerr << "failed to find register " << reg->reg_num << " " << reg->type; exit(1); }

    if(!r->allocated)
    {
        cerr << "double release of register " << reg->reg_num << " type " << reg->type;
        exit(1);
    }

    //Successful release
    r->allocated = false;
}
