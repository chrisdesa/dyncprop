#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "../Instr.hpp"
#include "../Data.hpp"
#include "../Home.hpp"
#include "../State.hpp"
#include "InstrGroupD1.hpp"

namespace Dyncprop {

  InstrGroupD1::InstrGroupD1(const uint8_t* ip): modrm(true, false, ip)
  {
    
  }

  InstrGroupD1::~InstrGroupD1()
  {
    
  }

  std::vector<Home> InstrGroupD1::inputs() const
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

  std::vector<Home> InstrGroupD1::outputs() const
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

  std::vector<uint8_t> InstrGroupD1::opcode() const
  {
    std::vector<uint8_t> rv;
    rv.push_back(0xD1);
    std::vector<uint8_t> suffix = modrm.opcode_suffix();
    for(int i = 0; i < suffix.size(); i++) {
      rv.push_back(suffix[i]);
    }
    return rv;
  }

  const Instr* InstrGroupD1::cprop(Home input, Data value) const
  {
    return NULL;
  }
  
  std::vector<Data> InstrGroupD1::emulate(std::vector<Data> ins) const
  {
    return Instr::emulate(ins);
  }
  
  const Instr* InstrGroupD1::canonicalize() const
  {
    uint8_t v_modrm = (3 << 6)|(modrm.opd2_reg << 3)|(2);
    return new InstrGroupD1(&v_modrm);
  }

  Instr* InstrGroupD1::parse(const uint8_t* ip)
  {
    uint8_t opc = *ip;
    if(opc == 0xD1) {
      return new InstrGroupD1(ip+1);
    }
    return NULL;
  }

  const char* InstrGroupD1::to_string() const
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
    sprintf(buf, "%s %s, 1", opid_name, opddst_name);
    delete[] opddst_name;
    return buf;
  }
  
}
