#include <stdlib.h>
#include <stdio.h>

#include "../Instr.hpp"
#include "../Data.hpp"
#include "../Home.hpp"
#include "../State.hpp"
#include "InstrImul.hpp"

namespace Dyncprop {

  InstrImul::InstrImul(const uint8_t* ip): modrm(true,true,ip)
  {
    
  }
  
  InstrImul::~InstrImul()
  {
    
  }

  std::vector<Home> InstrImul::inputs() const
  {
    std::vector<Home> rv;
    rv.push_back(modrm.opd_dst());
    rv.push_back(modrm.opd_src());
    return rv;
  }

  std::vector<Home> InstrImul::outputs() const
  {
    std::vector<Home> rv;
    rv.push_back(modrm.opd_dst());
    return rv;
  }

  std::vector<uint8_t> InstrImul::opcode() const
  {
    std::vector<uint8_t> rv;
    rv.push_back(0x0F);
    rv.push_back(0xAF);
    std::vector<uint8_t> suffix = modrm.opcode_suffix();
    for(int i = 0; i < suffix.size(); i++) {
      rv.push_back(suffix[i]);
    }
    return rv;
  }

  const Instr* InstrImul::cprop(Home input, Data value) const
  {
    return NULL;
  }
  
  const Instr* InstrImul::canonicalize() const
  {
    uint8_t v_modrm = (3 << 6)|(1 << 3)|(2);
    return new InstrImul(&v_modrm);
  }

  Instr* InstrImul::parse(const uint8_t* ip)
  {
    if((ip[0] == 0x0F)&&(ip[1] == 0xAF)) {
      return new InstrImul(ip+2);
    }
    return NULL;
  }

    const char* InstrImul::to_string() const
  {
    const char* opddst_name = modrm.opd_dst().to_string();
    const char* opdsrc_name = modrm.opd_src().to_string();
    char* buf = new char[64];
    sprintf(buf, "IMUL %s, %s", opddst_name, opdsrc_name);
    delete[] opddst_name;
    delete[] opdsrc_name;
    return buf;
  }
}
