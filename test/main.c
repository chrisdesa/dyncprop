#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include "dyncprop.h"

int nop(int x, int* y, int szy);

int foo(int x, int* y, int szy);
int fooO2(int x, int* y, int szy);

int intcmp(const void *a, const void *b)
{
  return ( (*(int*)a) - (*(int*)b) );
}

int main(int argc, char* argv[])
{
  clock_t clk_start, clk_end; int i;
  float foo_time, bar_time, fooO2_time, loop_time;
  const int niters = 100000000;
  
  srand( time(NULL) );
  
  int pysz = 64*1024;
  int *py = malloc(pysz*sizeof(int));
  for(int i = 0; i < pysz; i++) {
    py[i] = rand();
  }
  int x = py[0];
  qsort(py, pysz, sizeof(int), intcmp);
  
  dyncprop_init();  
  
  printf("Generating function...\n");
  int (*bar)(int,int*,int) = dyncprop(foo,"Ddd",py,pysz);
  printf("Function generated!\n");
  
  printf("Hello world!\n");
  printf("Testing original: %d\n", foo(x,py,pysz));
  printf("Testing prop 1:   %d\n", bar(x,py,pysz));
  printf("Testing prop 2:   %d\n", bar(x,NULL,7));
  printf("Testing prop 3:   %d\n", bar(x,py+1,80));
  
  printf("\n");
  
  int (*pfoo)(int,int*,int) = foo;
  int (*pfooO2)(int,int*,int) = fooO2;
  int (*pnop)(int,int*,int) = nop;
  
  clk_start = clock();
  for(i = 0; i < niters; i++) {
    x = i % RAND_MAX;
    pfoo(x,py,pysz);
  }
  clk_end = clock();
  foo_time = ((float)(clk_end - clk_start))/((float)(CLOCKS_PER_SEC))/((float)niters);
  
  //try to exercise all the paths of the code
  for(i = 0; i < 10000; i++) {
    x = rand();
    bar(x,py,pysz);
  }
  clk_start = clock();
  for(i = 0; i < niters; i++) {
    x = i % RAND_MAX;
    bar(x,py,pysz);
  }
  clk_end = clock();
  bar_time = ((float)(clk_end - clk_start))/((float)(CLOCKS_PER_SEC))/((float)niters);
  
  clk_start = clock();
  for(i = 0; i < niters; i++) {
    x = i % RAND_MAX;
    pfooO2(x,py,pysz);
  }
  clk_end = clock();
  fooO2_time = ((float)(clk_end - clk_start))/((float)(CLOCKS_PER_SEC))/((float)niters);
  
  clk_start = clock();
  for(i = 0; i < niters; i++) {
    x = i % RAND_MAX;
    pnop(x,py,pysz);
  }
  clk_end = clock();
  loop_time = ((float)(clk_end - clk_start))/((float)(CLOCKS_PER_SEC))/((float)niters);
  
  foo_time -= loop_time;
  fooO2_time -= loop_time;
  bar_time -= loop_time;
  
  printf("   foo time: %f ns\n", foo_time * 1e9);
  printf("foo O2 time: %f ns\n", fooO2_time * 1e9);
  printf("   bar time: %f ns\n", bar_time * 1e9);
  printf("    speedup: %02.1f%%\n", ((foo_time/bar_time) - 1.0f)*100.0f);
  printf("\n");
  
  return 0;
}

int foo(int x, int* y, int szy)
{
  int imin = 0;
  int imax = szy;
  while(imax > imin) {
    int i = (imin + imax)/2;
    if(y[i] == x) {
      return i;
    }
    else if(y[i] > x) {
      imax = i;
    }
    else {
      imin = i + 1;
    }
  }
  return -1;
  /*
  int z = 1;
  int r = 0;
  for(int i = 0; i < 11; i++) {
    if(y[i] == x) {
      return (i << 8);
    }
    z *= y[i];
    r += (x & i) + z;
  }
  return r & 255;
  */
}

int nop(int x, int* y, int szy)
{
  return 0;
}
