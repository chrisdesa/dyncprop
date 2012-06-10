#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "../Instr.hpp"
#include "../Data.hpp"
#include "../Home.hpp"
#include "../State.hpp"
#include "InstrGroup5IncDec.hpp"

namespace Dyncprop {

  InstrGroup5IncDec::InstrGroup5IncDec(const uint8_t* ip): modrm(true, false, ip)
  {
    if(!((modrm.opd2_reg == 0)||(modrm.opd2_reg == 1))) {
      fprintf(stderr, "Error: Invalid group 5 opcode (%s:%d).\n", __FILE__, __LINE__);
      exit(1);
    }
  }

  InstrGroup5IncDec::~InstrGroup5IncDec()
  {
    
  }

  std::vector<Home> InstrGroup5IncDec::inputs() const
  {
    std::vector<Home> rv;
    rv.push_back(modrm.opd_dst());
    return rv;
  }

  std::vector<Home> InstrGroup5IncDec::outputs() const
  {
    uint32_t opid = modrm.opd2_reg;
    std::vector<Home> rv;
    rv.push_back(modrm.opd_dst());
    rv.push_back(Home::HomeFlag(FLAG_P));
    rv.push_back(Home::HomeFlag(FLAG_A));
    rv.push_back(Home::HomeFlag(FLAG_Z));
    rv.push_back(Home::HomeFlag(FLAG_S));
    rv.push_back(Home::HomeFlag(FLAG_O));
    return rv;
  }

  std::vector<uint8_t> InstrGroup5IncDec::opcode() const
  {
    std::vector<uint8_t> rv;
    rv.push_back(0xFF);
    std::vector<uint8_t> suffix = modrm.opcode_suffix();
    for(int i = 0; i < suffix.size(); i++) {
      rv.push_back(suffix[i]);
    }
    return rv;
  }

  const Instr* InstrGroup5IncDec::cprop(Home input, Data value) const
  {
    return NULL;
  }
  
  std::vector<Data> InstrGroup5IncDec::emulate(std::vector<Data> ins) const
  {
    //we assume here that no one is going to be inc/dec the stack pointer,
    //since this would cause it to become unaligned
    return Instr::emulate(ins);
  }
  
  const Instr* InstrGroup5IncDec::canonicalize() const
  {
    uint8_t v_modrm = (3 << 6)|(modrm.opd2_reg << 3)|(2);
    return new InstrGroup5IncDec(&v_modrm);
  }

  Instr* InstrGroup5IncDec::parse(const uint8_t* ip)
  {
    uint8_t opc = *ip;
    if(opc == 0xFF) {
      uint8_t modrm_byte = *(ip+1);
      uint8_t opid = (modrm_byte >> 3) & 7;
      if((opid == 0)||(opid==1)) {
        return new InstrGroup5IncDec(ip+1);
      }
    }
    return NULL;
  }

  const char* InstrGroup5IncDec::to_string() const
  {
    const char* opid_name;
    switch(modrm.opd2_reg) {
      case 0:
        opid_name = "INC";
        break;
      case 1:
        opid_name = "DEC";
        break;
      default:
        fprintf(stderr, "Error: Invalid group 5 opcode (%s:%d).\n", __FILE__, __LINE__);
        exit(1);
    }
    const char* opddst_name = modrm.opd_dst().to_string();
    char* buf = new char[64];
    sprintf(buf, "%s %s", opid_name, opddst_name);
    delete[] opddst_name;
    return buf;
  }
  
}
