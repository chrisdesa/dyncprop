#pragma once

#include <stdint.h>
#include <vector>

namespace Dyncprop {

  enum DataState {
    DS_UNINITIALIZED,
    DS_VIRTUAL,
    DS_SYMBOLIC,
    DS_REAL,
    DS_STACK_PTR,
    DS_RET_ADDR,
    DS_RET_BX,
    DS_RET_BP,
    DS_RET_SI,
    DS_RET_DI
  };

  class Data {
  public:
    Data(): state(DS_UNINITIALIZED), value(0) { }
    Data(DataState state_, int32_t value_): state(state_), value(value_) { }
  public:
    DataState state;
    int32_t value;
  public:
    bool isvirtual() const;
    bool issymbolic() const;
  };

  Data data(DataState ds, int32_t v);
 
  const char* format_datastate(DataState ds);
}
