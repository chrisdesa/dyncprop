#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "../Instr.hpp"
#include "../Data.hpp"
#include "../Home.hpp"
#include "../State.hpp"
#include "InstrInc.hpp"

namespace Dyncprop {

  InstrInc::InstrInc(Register opd_): opd(opd_)
  {
    
  }

  InstrInc::~InstrInc()
  {
    
  }

  std::vector<Home> InstrInc::inputs() const
  {
    std::vector<Home> rv;
    rv.push_back(Home::HomeRegister(opd));
    return rv;
  }

  std::vector<Home> InstrInc::outputs() const
  {
    std::vector<Home> rv;
    rv.push_back(Home::HomeRegister(opd));
    rv.push_back(Home::HomeFlag(FLAG_P));
    rv.push_back(Home::HomeFlag(FLAG_A));
    rv.push_back(Home::HomeFlag(FLAG_Z));
    rv.push_back(Home::HomeFlag(FLAG_S));
    rv.push_back(Home::HomeFlag(FLAG_O));
    return rv;
  }

  std::vector<uint8_t> InstrInc::opcode() const
  {
    std::vector<uint8_t> rv;
    rv.push_back(0x40 + opd);
    return rv;
  }

  const Instr* InstrInc::cprop(Home input, Data value) const
  {
    return NULL;
  }
  
  std::vector<Data> InstrInc::emulate(std::vector<Data> ins) const
  {
    return Instr::emulate(ins);
  }
  
  const Instr* InstrInc::canonicalize() const
  {
    return this;
  }

  Instr* InstrInc::parse(const uint8_t* ip)
  {
    uint8_t opc = *ip;
    if((opc >= 0x40)&&(opc < 0x48)) {
      return new InstrInc((Register)(opc - 0x40));
    }
    return NULL;
  }

  const char* InstrInc::to_string() const
  {
    char* buf = new char[64];
    sprintf(buf, "INC %s", format_register(opd));
    return buf;
  }
  
}
