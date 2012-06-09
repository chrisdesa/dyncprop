#include <stdlib.h>
#include <stdio.h>

#include "../Instr.hpp"
#include "../Data.hpp"
#include "../Home.hpp"
#include "../State.hpp"
#include "InstrPush.hpp"

namespace Dyncprop {

  InstrPush::~InstrPush()
  {
    
  }
  
  std::vector<Home> InstrPush::inputs() const
  {
    std::vector<Home> rv;
    rv.push_back(Home::HomeRegister(opd));
    if(opd != REG_ESP) {
      rv.push_back(Home::HomeRegister(REG_ESP));
    }
    return rv;
  }

  std::vector<Home> InstrPush::outputs() const
  {
    std::vector<Home> rv;
    rv.push_back(Home::HomeRegister(REG_ESP));
    rv.push_back(Home::HomeMemory(REG_ESP, -4));
    return rv;
  }

  std::vector<uint8_t> InstrPush::opcode() const
  {
    std::vector<uint8_t> rv;
    rv.push_back((uint8_t)0x50 + (uint8_t)opd);
    return rv;
  }

  Instr* InstrPush::cprop(Home input, Data value) const
  {
    return NULL;
  }

  Instr* InstrPush::parse(const uint8_t* ip)
  {
    uint8_t opc = *ip;
    if((opc >= 0x50)&&(opc < 0x58)) {
      return new InstrPush((Register)(opc - 0x50));
    }
    return NULL;
  }
  
  const char* InstrPush::to_string() const
  {
    char* buf = new char[64];
    sprintf(buf, "PUSH %s", format_register(opd));
    return buf;
  }
  
  std::vector<Data> InstrPush::emulate(std::vector<Data> ins) const
  {
    std::vector<Data> rv;
    if(opd != REG_ESP) {
      fprintf(stderr, "Push::emulate(%s, %s)\n", format_datastate(ins[0].state), format_datastate(ins[1].state));
      Data dsp = ins[1];
      dsp.value -= 4;
      rv.push_back(dsp);
      rv.push_back(ins[0]);
    }
    else {
      Data dsp = ins[0];
      dsp.value -= 4;
      rv.push_back(dsp);
      dsp.value += 4;
      rv.push_back(dsp);
    }
    return rv;
  }
  
  InstrPush::InstrPush(Register opd_): opd(opd_)
  {
    
  }
}
