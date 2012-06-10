#pragma once

#include <stdint.h>
#include <vector>

#include "../Instr.hpp"

namespace Dyncprop {

  class Home;
  class Data;
  class State;

  /**
   * InstrMovImm
   **/
  
  class InstrMovImm: public Instr {    
  public:
    virtual std::vector<Home> inputs() const;
    virtual std::vector<Home> outputs() const;
    virtual std::vector<uint8_t> opcode() const;
    virtual const Instr* cprop(Home input, Data value) const;
    virtual const Instr* canonicalize() const;
    virtual std::vector<Data> emulate(std::vector<Data> ins) const;

    virtual const char* to_string() const;

  public:
    virtual ~InstrMovImm();
    InstrMovImm(Register r, int32_t offset, uint32_t imm_);

  private:
    InstrMovImm(const uint8_t *ip);
    InstrMovImm(const uint8_t *ip, uint32_t imm_);
    ModRM modrm;
    uint32_t imm;

  public:
    static Instr* parse(const uint8_t* ip);
  };

}
