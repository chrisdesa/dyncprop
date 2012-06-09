#pragma once

#include <stdint.h>
#include <vector>

#include "../Instr.hpp"

namespace Dyncprop {

  class Home;
  class Data;
  class State;

  /**
   * InstrImul
   **/
  
  class InstrImul: public Instr {    
  public:
    virtual std::vector<Home> inputs() const;
    virtual std::vector<Home> outputs() const;
    virtual std::vector<uint8_t> opcode() const;
    virtual const Instr* cprop(Home input, Data value) const;
    virtual const Instr* canonicalize() const;

    virtual const char* to_string() const;
    
  public:
    virtual ~InstrImul();
    
  private:
    InstrImul(const uint8_t* ip);
    ModRM modrm;

  public:
    static Instr* parse(const uint8_t* ip);
  };

}
