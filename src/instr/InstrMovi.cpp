#include <stdlib.h>
#include <stdio.h>

#include "../Instr.hpp"
#include "../Data.hpp"
#include "../Home.hpp"
#include "../State.hpp"
#include "InstrMovi.hpp"

namespace Dyncprop {

  InstrMovi::InstrMovi(Register r_, int32_t imm_): r(r_), imm(imm_)
  {
    
  }
  
  InstrMovi::~InstrMovi()
  {
    
  }

  std::vector<Home> InstrMovi::inputs() const
  {
    fprintf(stderr, "Error: Not implemented yet (%s:%d).\n", __FILE__, __LINE__);
    exit(1);
  }

  std::vector<Home> InstrMovi::outputs() const
  {
    fprintf(stderr, "Error: Not implemented yet (%s:%d).\n", __FILE__, __LINE__);
    exit(1);
  }

  std::vector<uint8_t> InstrMovi::opcode() const
  {
    std::vector<uint8_t> rv;
    uint32_t imm32 = (uint32_t)imm;
    rv.push_back(0xB8 + r);
    rv.push_back(imm32 & 0xFF); imm32 >>= 8;
    rv.push_back(imm32 & 0xFF); imm32 >>= 8;
    rv.push_back(imm32 & 0xFF); imm32 >>= 8;
    rv.push_back(imm32 & 0xFF); imm32 >>= 8;
    return rv;
  }

  const Instr* InstrMovi::cprop(Home input, Data value) const
  {
    return NULL;
  }

  const Instr* InstrMovi::canonicalize() const
  {
    return this;
  }

  Instr* InstrMovi::parse(const uint8_t* ip)
  {
    uint8_t opc = *ip;
    if((opc >= 0xB8)&&(opc < 0xC0)) {
      return new InstrMovi((Register)(opc - 0xB8), readimm32(ip+1));
    }
    return NULL;
  }
    
  const char* InstrMovi::to_string() const
  {
    char* buf = new char[64];
    sprintf(buf, "MOVI %s, %08x", format_register(r), imm);
    return buf;
  }
}
