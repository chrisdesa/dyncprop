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
    InstrALU1(uint32_t alu_opid, bool w, bool d, const uint8_t* ip);
    
  public:
    virtual std::vector<Home> inputs() const;
    virtual std::vector<Home> outputs() const;
    virtual std::vector<uint8_t> opcode() const;
    virtual const Instr* cprop(Home input, Data value) const;
    virtual std::vector<Data> emulate(std::vector<Data> ins) const;
    //virtual std::vector<int32_t> run(std::vector<int32_t> ins) const;
    virtual const Instr* canonicalize() const;
    
    virtual const char* to_string() const;
    
  public:
    virtual ~InstrALU1();
    
  private:
    ModRM modrm;
    uint32_t opid;
    
  public:
    static Instr* parse(const uint8_t* ip);
  };

}
