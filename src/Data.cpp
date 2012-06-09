#include <stdlib.h>
#include <stdio.h>

#include "Data.hpp"

namespace Dyncprop {
  
  Data data(DataState ds, int32_t v)
  {
    Data rv = Data(ds,v);
    return rv;
  }
 
  bool Data::isvirtual() const
  {
    return (state == DS_VIRTUAL)||(state == DS_REAL);
  }
  
  bool Data::issymbolic() const
  {
    return (state == DS_SYMBOLIC)||(state == DS_REAL);
  }
  
  const char* format_datastate(DataState ds)
  {
    switch(ds) {
#define FMT_CASE(x) case x: return #x;
    FMT_CASE(DS_UNINITIALIZED)
    FMT_CASE(DS_VIRTUAL)
    FMT_CASE(DS_SYMBOLIC)
    FMT_CASE(DS_REAL)
    FMT_CASE(DS_STACK_PTR)
    FMT_CASE(DS_RET_ADDR)
    FMT_CASE(DS_RET_BX)
    FMT_CASE(DS_RET_BP)
    FMT_CASE(DS_RET_SI)
    FMT_CASE(DS_RET_DI)
#undef FMT_CASE
      default:
        fprintf(stderr, "Error: Unrecognized datastate (%s:%d).\n", __FILE__, __LINE__);
        exit(1);
    }
  }
  
}
