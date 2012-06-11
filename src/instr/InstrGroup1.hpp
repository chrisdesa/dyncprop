#pragma once

#include <stdint.h>
#include <vector>

#include "../Instr.hpp"

namespace Dyncprop {

  class Home;
  class Data;
  class State;

  /**
   * InstrGroup1
   **/
  
  class InstrGroup1: public Instr {    
  public:
    virtual std::vector<Home> inputs() const;
    virtual std::vector<Home> outputs() const;
    virtual std::vector<uint8_t> opcode() const;
    virtual const Instr* cprop(Home input, Data value) const;
    virtual const Instr* canonicalize() const;
    virtual std::vector<Data> emulate(std::vector<Data> ins) const;

    virtual const char* to_string() const;

  public:
    virtual ~InstrGroup1();
    InstrGroup1(bool w, const uint8_t *ip, int32_t imm_);

  private:
    InstrGroup1(bool w, bool w_i_, const uint8_t *ip);
    ModRM modrm;
    int32_t imm;
    
  public:
    static Instr* parse(const uint8_t* ip);
  };

}
