#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "../Instr.hpp"
#include "../Data.hpp"
#include "../Home.hpp"
#include "../State.hpp"
#include "InstrALU1AI.hpp"
#include "InstrALU1.hpp"

namespace Dyncprop {

  InstrALU1AI::InstrALU1AI(uint32_t opid_, int32_t imm_): opid(opid_), imm(imm_)
  {
    
  }

  InstrALU1AI::~InstrALU1AI()
  {
    
  }

  std::vector<Home> InstrALU1AI::inputs() const
  {
    std::vector<Home> rv;
    rv.push_back(Home::HomeRegister(REG_EAX));
    if((opid == 2)||(opid == 3)) {
      rv.push_back(Home::HomeFlag(FLAG_C));
    }
    return rv;
  }

  std::vector<Home> InstrALU1AI::outputs() const
  {
    std::vector<Home> rv;
    if(opid != 7) {
      rv.push_back(Home::HomeRegister(REG_EAX));
    }
    rv.push_back(Home::HomeFlag(FLAG_C));
    rv.push_back(Home::HomeFlag(FLAG_P));
    rv.push_back(Home::HomeFlag(FLAG_A));
    rv.push_back(Home::HomeFlag(FLAG_Z));
    rv.push_back(Home::HomeFlag(FLAG_S));
    rv.push_back(Home::HomeFlag(FLAG_O));
    return rv;
  }

  std::vector<uint8_t> InstrALU1AI::opcode() const
  {
    std::vector<uint8_t> rv;
    uint8_t opc = (opid << 3) | (5);
    rv.push_back(opc);
  }

  const Instr* InstrALU1AI::cprop(Home input, Data value) const
  {
    return NULL;
  }
  
  const Instr* InstrALU1AI::canonicalize() const
  {
    //these ops can't be in memory
    return this;
  }

  Instr* InstrALU1AI::parse(const uint8_t* ip)
  {
    uint8_t opc = *ip;
    if((opc >= 0x00)&&(opc < 0x40)&&((opc & 7)==5)) {
      uint32_t opid = (opc >> 3) & 7;
      int32_t imm = (int32_t)(readimm32(ip+1));
      return new InstrALU1AI(opid, imm);
    }
    return NULL;
  }

  const char* InstrALU1AI::to_string() const
  {
    char* buf = new char[64];
    const char* opid_name = InstrALU1::opid_to_string(opid);
    sprintf(buf, "%sI %%eax, %08x", opid_name, imm);
    return buf;
  }
  
}
