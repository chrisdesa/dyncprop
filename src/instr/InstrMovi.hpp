#pragma once

#include <stdint.h>
#include <vector>

#include "../Instr.hpp"

namespace Dyncprop {

  class Home;
  class Data;
  class State;

  /**
   * InstrMovi
   **/
  
  class InstrMovi: public Instr {    
  public:
    virtual std::vector<Home> inputs() const;
    virtual std::vector<Home> outputs() const;
    virtual std::vector<uint8_t> opcode() const;
    virtual const Instr* cprop(Home input, Data value) const;
    virtual const Instr* canonicalize() const;

    virtual const char* to_string() const;
    
  public:
    virtual ~InstrMovi();
    
  private:
    InstrMovi(Register r_, int32_t imm_);
    Register r;
    int32_t imm;

  public:
    static Instr* parse(const uint8_t* ip);
    friend class Home;
  };

}
