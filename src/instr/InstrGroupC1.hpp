#pragma once

#include <stdint.h>
#include <vector>

#include "../Instr.hpp"

namespace Dyncprop {

  class Home;
  class Data;
  class State;

  /**
   * InstrGroupC1
   * 
   * For shift/rotate immediate instructions beginning with 0xC1
   **/
  
  class InstrGroupC1: public Instr {    
  public:
    virtual std::vector<Home> inputs() const;
    virtual std::vector<Home> outputs() const;
    virtual std::vector<uint8_t> opcode() const;
    virtual const Instr* cprop(Home input, Data value) const;
    virtual const Instr* canonicalize() const;
    virtual std::vector<Data> emulate(std::vector<Data> ins) const;

    virtual const char* to_string() const;

  public:
    virtual ~InstrGroupC1();

  private:
    InstrGroupC1(const uint8_t* ip);
    InstrGroupC1(const uint8_t* ip, int8_t imm_);
    ModRM modrm;
    int8_t imm;

  public:
    static Instr* parse(const uint8_t* ip);
  };

}
