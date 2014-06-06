/* main.cpp */

#include <stdio.h>
#include <iostream>
#include <string>

using namespace std;

// prototype of bison-generated parser function
extern int yyparse();

int main(int argc, char **argv)
{
    if ((argc > 1) && (freopen(argv[1], "r", stdin) == NULL))
    {
        cout << argv[0] << ": File " << argv[1] << "cannot be opened." << endl;
        return 1;
    }

    cout << "\t.text" << endl;
    cout << "\t.globl main" << endl;
    cout << "\t.globl dat" << endl;
    cout << "\tla $gp dat" << endl;
    cout << "\tadd $fp $sp $zero #initialize fp" << endl;
    cout << "\tj main" << endl;
    cout << "\t.data" << endl;
    cout << "dat:" << endl;

    yyparse();

    return 0;
}
