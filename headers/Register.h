#ifndef __REGISTER_H__
#define __REGISTER_H__

#include <vector>
#include <string>

class Register
{
    public:
        enum RegisterType
        {
            Temp,
            Save,
        };

        static int reservedRegisters();
        static Register* FindRegister(RegisterType type);
        static void ReleaseRegister(Register* reg);

        std::string name();
        std::string toString();

    private:
        Register(int reg_num, RegisterType type);
        virtual ~Register();
        int reg_num;
        bool allocated;
        RegisterType type;

        static std::vector<Register*> temp_registers;
        static std::vector<Register*> save_registers;
};

#endif //__REGISTER_H__
