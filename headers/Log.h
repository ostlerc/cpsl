#ifndef __LOG_H__
#define __LOG_H__

#include <iostream>
#include <fstream>

class Log
{
    std::ofstream byname;
    public:
    std::ostream& out;
    Log(std::ostream& stream = std::cout) : byname(), out(stream) { }
    Log(std::string filename) : byname(filename), out(this->byname)
    {
        if (!out)
        {
            std::cerr << "cpsl: File " << filename << " cannot be opened." << std::endl;
            exit(1);
        }
    }
};

#endif //__LOG_H__
