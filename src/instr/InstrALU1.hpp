#pragma once

#include <stdint.h>
#include <vector>

#include "../Instr.hpp"

namespace Dyncprop {

  class Home;
  class Data;
  class State;

  /**
   * InstrALU1
   * 
   * This class of instructions includes the following ops
   *   ADD
   *   OR
   *   ADC
   *   SBB
   *   AND
   *   SUB
   *   XOR
   *   CMP
   * 
   * It is closely related to the instruction groups:
   *   ALUI1 - alu ops with operands EAX and immediate
   *   GRP1 - alu ops with operands (...) and immediate
   **/
  
  class InstrALU1: public Instr {    
  public:
    virtual std::vector<Home> inputs() const;
    virtual std::vector<Home> outputs() const;
    virtual std::vector<uint8_t> opcode() const;
    virtual Instr* cprop(Home input, Data value) const;
    
  public:
    virtual ~InstrALU1();
    
  public:
    static Instr* parse(const uint8_t* ip);
  };

}
