#pragma once

#include <stdint.h>
#include <vector>

#include "../Instr.hpp"

namespace Dyncprop {

  class Home;
  class Data;
  class State;

  /**
   * InstrALU1AI
   **/
  
  class InstrALU1AI: public Instr {    
  public:
    virtual std::vector<Home> inputs() const;
    virtual std::vector<Home> outputs() const;
    virtual std::vector<uint8_t> opcode() const;
    virtual const Instr* cprop(Home input, Data value) const;
    virtual const Instr* canonicalize() const;

    virtual const char* to_string() const;

  public:
    virtual ~InstrALU1AI();

  public:
    InstrALU1AI(uint32_t opid_, int32_t imm_);
    
  public:
    uint32_t opid;
    int32_t imm;

  public:
    static Instr* parse(const uint8_t* ip);
  };

}
