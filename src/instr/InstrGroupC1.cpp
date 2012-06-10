#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "../Instr.hpp"
#include "../Data.hpp"
#include "../Home.hpp"
#include "../State.hpp"
#include "InstrGroupC1.hpp"

namespace Dyncprop {

  InstrGroupC1::InstrGroupC1(const uint8_t* ip): modrm(true, false, ip)
  {
    ip += modrm.opcode_suffix().size();
    imm = (int8_t)readimm8(ip);
  }
  
  InstrGroupC1::InstrGroupC1(const uint8_t* ip, int8_t imm_): modrm(true, false, ip), imm(imm_)
  {
    
  }

  InstrGroupC1::~InstrGroupC1()
  {
    
  }

  std::vector<Home> InstrGroupC1::inputs() const
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

  std::vector<Home> InstrGroupC1::outputs() const
  {
    std::vector<Home> rv;
    rv.push_back(modrm.opd_dst());
    rv.push_back(Home::HomeFlag(FLAG_C));
    rv.push_back(Home::HomeFlag(FLAG_P));
    rv.push_back(Home::HomeFlag(FLAG_A));
    rv.push_back(Home::HomeFlag(FLAG_Z));
    rv.push_back(Home::HomeFlag(FLAG_S));
    rv.push_back(Home::HomeFlag(FLAG_O));
    return rv;
  }

  std::vector<uint8_t> InstrGroupC1::opcode() const
  {
    std::vector<uint8_t> rv;
    rv.push_back(0xC1);
    std::vector<uint8_t> suffix = modrm.opcode_suffix();
    for(int i = 0; i < suffix.size(); i++) {
      rv.push_back(suffix[i]);
    }
    rv.push_back((uint8_t)imm);
    return rv;
  }

  const Instr* InstrGroupC1::cprop(Home input, Data value) const
  {
    return NULL;
  }
  
  std::vector<Data> InstrGroupC1::emulate(std::vector<Data> ins) const
  {
    return Instr::emulate(ins);
  }
  
  const Instr* InstrGroupC1::canonicalize() const
  {
    uint8_t v_modrm = (3 << 6)|(modrm.opd2_reg << 3)|(2);
    return new InstrGroupC1(&v_modrm, imm);
  }

  Instr* InstrGroupC1::parse(const uint8_t* ip)
  {
    uint8_t opc = *ip;
    if(opc == 0xC1) {
      return new InstrGroupC1(ip+1);
    }
    return NULL;
  }

  const char* InstrGroupC1::to_string() const
  {
    const char* opid_name;
    switch(modrm.opd2_reg) {
      case 0: opid_name = "ROL"; break;
      case 1: opid_name = "ROR"; break;
      case 2: opid_name = "RCL"; break;
      case 3: opid_name = "RCR"; break;
      case 4: opid_name = "SHL"; break;
      case 5: opid_name = "SHR"; break;
      case 6: opid_name = "SAL"; break;
      case 7: opid_name = "SAR"; break;
      default:
        fprintf(stderr, "Error: Invalid group C1 opcode (%s:%d).\n", __FILE__, __LINE__);
        exit(1);
    }
    const char* opddst_name = modrm.opd_dst().to_string();
    char* buf = new char[64];
    sprintf(buf, "%s %s, %02x", opid_name, opddst_name, (uint32_t)imm);
    delete[] opddst_name;
    return buf;
  }
  
}
