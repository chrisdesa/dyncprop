#pragma once

#include <stdint.h>
#include <vector>

#include "Data.hpp"

#define STACK_MAX (4*1024)

namespace Dyncprop {

  enum Register {
    //general purpose registers
    REG_EAX = 0,
    REG_ECX = 1,
    REG_EDX = 2,
    REG_EBX = 3,
    //pointer registers
    REG_ESP = 4,
    REG_EBP = 5,
    //index registers
    REG_ESI = 6,
    REG_EDI = 7,
    //maximum value of enum
    REG_ENUM_MAX = 8,
    //no register
    REG_NONE = -1
  };
  
  enum Flag {
    FLAG_C = 0,
    FLAG_P = 2,
    FLAG_A = 4,
    FLAG_Z = 6,
    FLAG_S = 7,
    FLAG_O = 11,
    FLAG_ENUM_MAX = 16
  };
  
  class State {
  public:
    State();
  public:
    //data entries
    Data regs[REG_ENUM_MAX];
    Data flags[FLAG_ENUM_MAX];
    std::vector<Data> stack;
  public:
    //instruction pointer
    const uint8_t* ip;
  public:
    //emission buffer
    std::vector<uint8_t> emitbuf;
  public:
    //step the simulator
    bool step();
  public:
    //referencing the stack
    Data& stack_at(int32_t offset);
  public:
    //dump core and exit
    void dump();
  };

  const char* format_register(Register r);
  const char* format_flag(Flag f);
  
  bool register_valid(Register r);
  bool flag_valid(Flag f);
}