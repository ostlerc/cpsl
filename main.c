/* main.cc */

#include <stdio.h>

// prototype of bison-generated parser function
int yyparse();

int main(int argc, char **argv)
{
  if ((argc > 1) && (freopen(argv[1], "r", stdin) == NULL))
  {
    printf("%s: File %s cannot be opened.\n", argv[0], argv[1]);
    return 1;
  }
  
  yyparse();

  return 0;
}

