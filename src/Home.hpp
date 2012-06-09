#pragma once

#include <stdint.h>
#include <vector>

#include "State.hpp"

namespace Dyncprop {
  
  class Data;
  class State;

  class Home {
  private:
    Home();
  public:
    Home(const Home& h);
    Home& operator =(const Home& h);
    bool operator ==(const Home& h) const;
    bool operator !=(const Home& h) const;
  public:
    enum HomeMode {
      HM_REGISTER,
      HM_FLAG,
      HM_MEMORY,
      HM_MEMORY_CONST
    } mode;
    Register r;
    Flag f;
    int32_t offset;
  public:
    void realize(State& s) const;
    Data get(State& s) const;
    Data& access(State& s) const;
  public:
    static Home HomeRegister(Register r);
    static Home HomeFlag(Flag f);
    static Home HomeMemory(Register r, int32_t offset);
    static Home HomeMemoryConst(int32_t offset);
  public:
    const char* to_string() const;
  };
  
}
