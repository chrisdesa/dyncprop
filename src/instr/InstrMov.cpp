#include <stdlib.h>
#include <stdio.h>

#include "../Instr.hpp"
#include "../Data.hpp"
#include "../Home.hpp"
#include "../State.hpp"
#include "InstrMov.hpp"

namespace Dyncprop {

  InstrMov::InstrMov(bool w_, bool d_, const uint8_t* ip): modrm(w_,d_,ip)
  {
    
  }
  
  InstrMov::~InstrMov()
  {
    
  }

  std::vector<Home> InstrMov::inputs() const
  {
    std::vector<Home> rv;
    rv.push_back(modrm.opd_src());
    return rv;
  }

  std::vector<Home> InstrMov::outputs() const
  {
    std::vector<Home> rv;
    rv.push_back(modrm.opd_dst());
    return rv;
  }

  std::vector<uint8_t> InstrMov::opcode() const
  {
    std::vector<uint8_t> rv;
    uint8_t opc = 0x88 | (modrm.w ? 1 : 0) | (modrm.d ? 2 : 0);
    rv.push_back(opc);
    std::vector<uint8_t> suffix = modrm.opcode_suffix();
    for(int i = 0; i < suffix.size(); i++) {
      rv.push_back(suffix[i]);
    }
    return rv;
  }

  const Instr* InstrMov::cprop(Home input, Data value) const
  {
    return NULL;
  }

  const Instr* InstrMov::canonicalize() const
  {
    uint8_t v_modrm = (3 << 6)|(1 << 3)|(2);
    return new InstrMov(modrm.w, modrm.d, &v_modrm);
  }
  /*
  std::vector<int32_t> InstrMov::run(std::vector<int32_t> ins) const
  {
    if(ins.size() != 1) {
      fprintf(stderr, "Error: Vector length mismatch (%s:%d).\n", __FILE__, __LINE__);
      exit(1);
    }
    std::vector<int32_t> outs;
    outs.push_back(ins[0]);
    return outs;
  }  
  */
  std::vector<Data> InstrMov::emulate(std::vector<Data> ins) const
  {
    if(ins.size() != 1) {
      fprintf(stderr, "Error: Vector length mismatch (%s:%d).\n", __FILE__, __LINE__);
      exit(1);
    }
    std::vector<Data> outs;
    outs.push_back(ins[0]);
    return outs;
  }

  Instr* InstrMov::parse(const uint8_t* ip)
  {
    uint8_t opc = *ip;
    if((opc >= 0x88)&&(opc < 0x8C)) {
      return new InstrMov(opc & 1, !!(opc & 2), ip+1);
    }
    return NULL;
  }
  
  const char* InstrMov::to_string() const
  {
    const char* opddst_name = modrm.opd_dst().to_string();
    const char* opdsrc_name = modrm.opd_src().to_string();
    char* buf = new char[256];
    sprintf(buf, "MOV %s, %s", opddst_name, opdsrc_name);
    delete[] opddst_name;
    delete[] opdsrc_name;
    return buf;
  }
}
