/* main.cpp */

#include <stdio.h>
#include <iostream>
#include <string>

using namespace std;

// prototype of bison-generated parser function
extern int yyparse();

string* expression (int i)
{
    string *a = new string("abc");
    cout << "int!" << i << endl;
    return a;
}

string * expression(string* c)
{
    cout << "char*!" << c->c_str() << endl;
    return c;
}

int main(int argc, char **argv)
{
    if ((argc > 1) && (freopen(argv[1], "r", stdin) == NULL))
    {
        std::cout << argv[0] << ": File " << argv[1] << "cannot be opened." << std::endl;
        return 1;
    }

    yyparse();

    return 0;
}
