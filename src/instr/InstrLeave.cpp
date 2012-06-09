#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "../Instr.hpp"
#include "../Data.hpp"
#include "../Home.hpp"
#include "../State.hpp"
#include "InstrLeave.hpp"
#include <vector>

namespace Dyncprop {

  InstrLeave::InstrLeave()
  {
    
  }

  InstrLeave::~InstrLeave()
  {
    
  }

  std::vector<Home> InstrLeave::inputs() const
  {
    std::vector<Home> rv;
    rv.push_back(Home::HomeRegister(REG_EBP));
    rv.push_back(Home::HomeMemory(REG_EBP, 0));
    return rv;
  }

  std::vector<Home> InstrLeave::outputs() const
  {
    std::vector<Home> rv;
    rv.push_back(Home::HomeRegister(REG_ESP));
    rv.push_back(Home::HomeRegister(REG_EBP));
    return rv;
  }

  std::vector<uint8_t> InstrLeave::opcode() const
  {
    std::vector<uint8_t> rv;
    rv.push_back(0xc9);
    return rv;
  }

  const Instr* InstrLeave::cprop(Home input, Data value) const
  {
    return NULL;
  }
  
  std::vector<Data> InstrLeave::emulate(std::vector<Data> ins) const
  {
    std::vector<Data> rv;
    Data dsp = ins[0];
    dsp.value += 4;
    rv.push_back(dsp);
    rv.push_back(ins[1]);
    return rv;
  }
  
  const Instr* InstrLeave::canonicalize() const
  {
    fprintf(stderr, "Error: Not implemented yet (%s:%d).\n", __FILE__, __LINE__);
    exit(1);
  }

  Instr* InstrLeave::parse(const uint8_t* ip)
  {
    int opc = *ip;
    if(opc == 0xc9)
      return new InstrLeave();
    else 
      return NULL;
  }

  const char* InstrLeave::to_string() const
  {
    char *buff = new char[16];
    sprintf(buff, "LEAVE");
    return buff;
  }
  
}
