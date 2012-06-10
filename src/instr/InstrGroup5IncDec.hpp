#pragma once

#include <stdint.h>
#include <vector>

#include "../Instr.hpp"

namespace Dyncprop {

  class Home;
  class Data;
  class State;

  /**
   * InstrGroup5IncDec
   **/
  
  class InstrGroup5IncDec: public Instr {    
  public:
    virtual std::vector<Home> inputs() const;
    virtual std::vector<Home> outputs() const;
    virtual std::vector<uint8_t> opcode() const;
    virtual const Instr* cprop(Home input, Data value) const;
    virtual const Instr* canonicalize() const;
    virtual std::vector<Data> emulate(std::vector<Data> ins) const;

    virtual const char* to_string() const;

  public:
    virtual ~InstrGroup5IncDec();

  private:
    InstrGroup5IncDec(const uint8_t* ip);
    ModRM modrm;

  public:
    static Instr* parse(const uint8_t* ip);
  };

}
