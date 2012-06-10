#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "../Instr.hpp"
#include "../Data.hpp"
#include "../Home.hpp"
#include "../State.hpp"
#include "InstrJmp.hpp"

namespace Dyncprop {

  InstrJmp::InstrJmp(int32_t imm_, bool absolute_): imm(imm_), absolute(absolute_)
  {
    
  }

  InstrJmp::~InstrJmp()
  {
    
  }

  std::vector<Home> InstrJmp::inputs() const
  {
    fprintf(stderr, "Error: Not implemented yet (%s:%d).\n", __FILE__, __LINE__);
    exit(1);
  }

  std::vector<Home> InstrJmp::outputs() const
  {
    fprintf(stderr, "Error: Not implemented yet (%s:%d).\n", __FILE__, __LINE__);
    exit(1);
  }

  std::vector<uint8_t> InstrJmp::opcode() const
  {
    std::vector<uint8_t> rv;
    bool w_i = (absolute)||(imm < -128)||(imm >= 128);
    uint8_t opc = (absolute) ? 0xEA : ((w_i) ? 0xE9 : 0xEB);
    rv.push_back(opc);
    if(w_i) {
      uint32_t imm32 = (uint32_t)imm;
      rv.push_back((uint8_t)imm32); imm32 >>= 8;
      rv.push_back((uint8_t)imm32); imm32 >>= 8;
      rv.push_back((uint8_t)imm32); imm32 >>= 8;
      rv.push_back((uint8_t)imm32); imm32 >>= 8;
    }
    else {
      uint8_t imm8 = (uint8_t)imm;
      rv.push_back(imm8);
    }
    return rv;
  }

  const Instr* InstrJmp::cprop(Home input, Data value) const
  {
    return NULL;
  }
  
  std::vector<Data> InstrJmp::emulate(std::vector<Data> ins) const
  {
    return Instr::emulate(ins);
  }
  
  const Instr* InstrJmp::canonicalize() const
  {
    fprintf(stderr, "Error: Not implemented yet (%s:%d).\n", __FILE__, __LINE__);
    exit(1);
  }
  
  bool InstrJmp::process(State& s) const
  {
    if(absolute) {
      s.ip = (const uint8_t*)imm;
    }
    else {
      s.ip += imm;
    }
    return false;
  }

  Instr* InstrJmp::parse(const uint8_t* ip)
  {
    uint8_t opc = *ip;
    if(opc == 0xe9) {
      return new InstrJmp((int32_t)readimm32(ip+1), false);
    }
    else if(opc == 0xea)
    {
      return new InstrJmp((int32_t)readimm32(ip+1), true);
    }
    else if(opc == 0xeb)
    {
      return new InstrJmp((int32_t)((int8_t)readimm8(ip+1)), false);
    }
    else
      return NULL;
  }

  const char* InstrJmp::to_string() const
  {
    char* buf = new char[64];
    if(absolute) {
      sprintf(buf, "JMP <%08x>", imm);
    }
    else {
      if(imm > 0) {
        sprintf(buf, "JMP <%%ip+%08x>", imm);
      }
      else {
        sprintf(buf, "JMP <%%ip-%08x>", -imm);
      }
    }
    return buf;
  }
  
}
