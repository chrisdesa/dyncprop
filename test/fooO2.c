#include <stdlib.h>
#include <stdio.h>
#include <time.h>

int fooO2(int x, int* y, int szy);

int fooO2(int x, int* y, int szy)
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
}
