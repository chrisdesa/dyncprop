#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include "dyncprop.h"

int foo(int x, int y);

int main(int argc, char* argv[])
{
  clock_t clk_start, clk_end; int i;
  float foo_time, bar_time;
  const int niters = 10000000;
  
  dyncprop_init();
  
  printf("Hello world!\n");
  printf("Testing original: %d\n", foo(3,5));
  
  printf("Generating function...\n");
  int (*bar)(int,int) = dyncprop(foo,"Dd",5);
  printf("Function generated!\n");
  
  printf("Testing prop 1:   %d\n", bar(3,5));
  printf("Testing prop 2:   %d\n", bar(3,1));
  printf("Testing prop 3:   %d\n", bar(3,0));
  
  printf("\n");
  
  int (*pfoo)(int,int) = foo;
  
  
  clk_start = clock();
  for(i = 0; i < niters; i++) {
    pfoo(3,4);
  }
  clk_end = clock();
  foo_time = ((float)(clk_end - clk_start))/((float)(CLOCKS_PER_SEC))/((float)niters);
  
  
  clk_start = clock();
  for(i = 0; i < niters; i++) {
    bar(3,4);
  }
  clk_end = clock();
  bar_time = ((float)(clk_end - clk_start))/((float)(CLOCKS_PER_SEC))/((float)niters);
  
  printf("foo time: %f ns\n", foo_time * 1e9);
  printf("bar time: %f ns\n", bar_time * 1e9);
  printf(" speedup: %02.1f%%\n", ((foo_time/bar_time) - 1.0f)*100.0f);
  printf("\n");
  
  return 0;
}

int foo(int x, int y)
{
  int z = 1;
  int r = 0;
  for(int i = 0; i < 77; i++) {
    z *= y;
    r += (x & i) + z;
  }
  return r & 255;
}