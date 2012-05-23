#include <stdlib.h>
#include <stdio.h>

#include "dyncprop.h"

int foo(int x, int y);

int main(int argc, char* argv[])
{
  printf("Hello world!\n");
  printf("Testing original: 3 + 4 + 4 = %d\n", foo(3,4));
  
  printf("Generating function...\n");
  int (*bar)(int,int) = dyncprop(foo,"Dd",4);
  printf("Function generated!\n");
  
  printf("Testing prop 1:   3 + 4 + 4 = %d\n", bar(3,4));
  printf("Testing prop 2:   3 + 4 + 4 = %d\n", bar(3,1));
  printf("Testing prop 3:   3 + 4 + 4 = %d\n", bar(3,0));
  
  return 0;
}

int foo(int x, int y)
{
  return x + y + y;
}