/* main.cpp */

#include <stdio.h>
#include <iostream>
#include <string>
#include <unistd.h>
#include "Log.h"

using namespace std;

// prototype of bison-generated parser function
extern int yyparse();
Log* cpsl_log = 0;

int main(int argc, char **argv)
{
    if ((argc == 2) && (freopen(argv[1], "r", stdin) == NULL))
    {
        cerr << argv[0] << ": File " << argv[1] << " cannot be opened." << endl;
        return 1;
    }

    int c;
    while((c =  getopt(argc, argv, "i:o:")) != EOF)
    {
        switch (c)
        {
            case 'i':
                if(freopen(optarg, "r", stdin) == NULL)
                {
                    cerr << argv[0] << ": File " << argv[1] << "cannot be opened." << endl;
                    return 1;
                }
                break;
            case 'o':
                cpsl_log = new Log(optarg);
                break;
            default:
                cerr << "unsupported option '" << (char)c << "'" << endl;
                exit(1);
                break;
        }
    }

    if(!cpsl_log)
        cpsl_log = new Log();

    cpsl_log->out << "\t.text" << endl;
    cpsl_log->out << "\t.globl main" << endl;
    cpsl_log->out << "\t.globl dat" << endl;
    cpsl_log->out << "\tla $gp dat" << endl;
    cpsl_log->out << "\tadd $fp $sp $zero #initialize fp" << endl;
    cpsl_log->out << "\tj main" << endl;
    cpsl_log->out << "\t.data" << endl;
    cpsl_log->out << "dat:" << endl;

    yyparse();

    return 0;
}
