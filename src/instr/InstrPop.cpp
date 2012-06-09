#include <stdlib.h>
#include <stdio.h>

#include "../Instr.hpp"
#include "../Data.hpp"
#include "../Home.hpp"
#include "../State.hpp"
#include "InstrPop.hpp"

namespace Dyncprop {

  InstrPop::~InstrPop()
  {
    
  }

  std::vector<Home> InstrPop::inputs() const
  {
    std::vector<Home> rv;
    rv.push_back(Home::HomeRegister(REG_ESP));
    rv.push_back(Home::HomeMemory(REG_ESP, 0));
    return rv;
  }

  std::vector<Home> InstrPop::outputs() const
  {
    std::vector<Home> rv;
    rv.push_back(Home::HomeRegister(opd));
    if(opd != REG_ESP) {
      rv.push_back(Home::HomeRegister(REG_ESP));
    }
    return rv;
  }

  std::vector<uint8_t> InstrPop::opcode() const
  {
    std::vector<uint8_t> rv;
    rv.push_back((uint8_t)0x58 + (uint8_t)opd);
    return rv;
  }
    
  const char* InstrPop::to_string() const
  {
    char* buf = new char[64];
    sprintf(buf, "POP %s", format_register(opd));
    return buf;
  }
  
  std::vector<Data> InstrPop::emulate(std::vector<Data> ins) const
  {
    std::vector<Data> rv;
    if(opd != REG_ESP) {
      Data dsp = ins[0];
      dsp.value += 4;
      rv.push_back(ins[1]);
      rv.push_back(dsp);
    }
    else {
      rv.push_back(ins[1]);
    }
    return rv;
  }

  Instr* InstrPop::cprop(Home input, Data value) const
  {
    return NULL;
  }

  Instr* InstrPop::parse(const uint8_t* ip)
  {    
    uint8_t opc = *ip;
    if((opc >= 0x58)&&(opc < 0x60)) {
      return new InstrPop((Register)(opc - 0x58));
    }
    return NULL;
  }
  
  InstrPop::InstrPop(Register opd_): opd(opd_)
  {
    
  }
}
