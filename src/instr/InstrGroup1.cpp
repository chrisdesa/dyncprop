#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <vector>

#include "../Instr.hpp"
#include "../Data.hpp"
#include "../Home.hpp"
#include "../State.hpp"
#include "InstrGroup1.hpp"
#include "InstrALU1.hpp"

namespace Dyncprop {

  InstrGroup1::InstrGroup1(bool w, bool w_i, const uint8_t *ip): modrm(w, false, ip)
  {
    ip += modrm.opcode_suffix().size();
    if(w_i) {
      imm = (int32_t)readimm32(ip);
    }
    else {
      imm = (int32_t)((int8_t)readimm8(ip));
    }
  }
  
  InstrGroup1::InstrGroup1(bool w, const uint8_t *ip, int32_t imm_): imm(imm_), modrm(w, false, ip)
  {
    
  }

  InstrGroup1::~InstrGroup1()
  {
    
  }

  std::vector<Home> InstrGroup1::inputs() const
  {
    uint32_t opid = modrm.opd2_reg;
    std::vector<Home> rv;
    rv.push_back(modrm.opd_dst());
    if((opid == 2)||(opid == 3)) {
      //these ops depend on the carry flag
      rv.push_back(Home::HomeFlag(FLAG_C));
    }
    return rv;
  }

  std::vector<Home> InstrGroup1::outputs() const
  {
    uint32_t opid = modrm.opd2_reg;
    std::vector<Home> rv;
    if(opid != 7) {
      //if it's not CMP
      rv.push_back(modrm.opd_dst());
    }
    rv.push_back(Home::HomeFlag(FLAG_C));
    rv.push_back(Home::HomeFlag(FLAG_P));
    rv.push_back(Home::HomeFlag(FLAG_A));
    rv.push_back(Home::HomeFlag(FLAG_Z));
    rv.push_back(Home::HomeFlag(FLAG_S));
    rv.push_back(Home::HomeFlag(FLAG_O));
    return rv;
  }

  std::vector<uint8_t> InstrGroup1::opcode() const
  {
    std::vector<uint8_t> rv;
    bool w_i = (!modrm.w)||(imm < -128)||(imm >= 128); //set if 4-byte
    uint8_t opc = 0x80 | (modrm.w ? 1 : 0) | (w_i ? 0 : 2);
    rv.push_back(opc);
    std::vector<uint8_t> suffix = modrm.opcode_suffix();
    for(int i = 0; i < suffix.size(); i++) {
      rv.push_back(suffix[i]);
    }
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

  const Instr* InstrGroup1::cprop(Home input, Data value) const
  {
    return NULL;
  }
  
  std::vector<Data> InstrGroup1::emulate(std::vector<Data> ins) const
  {
    return Instr::emulate(ins);
  }
  
  const Instr* InstrGroup1::canonicalize() const
  {
    uint32_t opid = modrm.opd2_reg;
    uint8_t v_modrm = (3 << 6)|(opid << 3)|(2);
    return new InstrGroup1(modrm.w, &v_modrm, imm);
  }

  Instr* InstrGroup1::parse(const uint8_t* ip)
  {
    int opc = *ip;
    if(opc >= 0x80 && opc < 0x84)
      return new InstrGroup1(opc & 1, opc == 0x81, ip+1);
    else
      return NULL;
  }

  const char* InstrGroup1::to_string() const
  {
    const char* opid_name = InstrALU1::opid_to_string(modrm.opd2_reg);
    const char* opddst_name = modrm.opd_dst().to_string();
    char* buf = new char[256];
    sprintf(buf, "%s %s, %08x", opid_name, opddst_name, imm);
    delete[] opddst_name;
    return buf;
  }
  
}
