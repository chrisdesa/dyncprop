#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "../Instr.hpp"
#include "../Data.hpp"
#include "../Home.hpp"
#include "../State.hpp"
#include "InstrMovImm.hpp"

namespace Dyncprop {

  InstrMovImm::InstrMovImm(const uint8_t *ip): modrm(true, false, ip)
  {
    if(modrm.opd2_reg != 0) {
      fprintf(stderr, "Error: Decoding error (%s:%d).\n", __FILE__, __LINE__);
      exit(1);
    }
    ip += modrm.opcode_suffix().size();
    imm = (uint32_t)readimm32(ip);
  }
  
  InstrMovImm::InstrMovImm(const uint8_t *ip, uint32_t imm_): modrm(true, false, ip), imm(imm_)
  {
    if(modrm.opd2_reg != 0) {
      fprintf(stderr, "Error: Decoding error (%s:%d).\n", __FILE__, __LINE__);
      exit(1);
    }
  }
  
  InstrMovImm::InstrMovImm(Register r, int32_t offset, uint32_t imm_): imm(imm_)
  {
    modrm.m = true;
    modrm.opd2_reg = (Register)0;
    modrm.opd1_reg = r;
    modrm.opd1_offset = offset;
    modrm.w = true;
    modrm.d = false;
  }

  InstrMovImm::~InstrMovImm()
  {
    
  }

  std::vector<Home> InstrMovImm::inputs() const
  {
    std::vector<Home> rv;
    return rv;
  }

  std::vector<Home> InstrMovImm::outputs() const
  {
    std::vector<Home> rv;
    rv.push_back(modrm.opd_dst());
    return rv;
  }

  std::vector<uint8_t> InstrMovImm::opcode() const
  {
    std::vector<uint8_t> rv;
    uint8_t opc = 0xC7;
    rv.push_back(opc);
    std::vector<uint8_t> suffix = modrm.opcode_suffix();
    for(int i = 0; i < suffix.size(); i++) {
      rv.push_back(suffix[i]);
    }
    uint32_t imm32 = (uint32_t)imm;
    rv.push_back((uint8_t)imm32); imm32 >>= 8;
    rv.push_back((uint8_t)imm32); imm32 >>= 8;
    rv.push_back((uint8_t)imm32); imm32 >>= 8;
    rv.push_back((uint8_t)imm32); imm32 >>= 8;
    return rv;
  }

  const Instr* InstrMovImm::cprop(Home input, Data value) const
  {
    return NULL;
  }
  
  std::vector<Data> InstrMovImm::emulate(std::vector<Data> ins) const
  {
    return Instr::emulate(ins);
  }
  
  const Instr* InstrMovImm::canonicalize() const
  {
    uint8_t v_modrm = (3 << 6)|(0 << 3)|(2);
    return new InstrMovImm(&v_modrm, imm);
  }

  Instr* InstrMovImm::parse(const uint8_t* ip)
  {
    int opc = *ip;
    if(opc == 0xC7)
      return new InstrMovImm(ip+1);
    else 
      return NULL;
  }

  const char* InstrMovImm::to_string() const
  {
    char *buff = new char[32];
    const char* opddst_name = modrm.opd_dst().to_string();
    sprintf(buff, "MOVL %s, %08x", opddst_name, imm);
    delete[] opddst_name;
    return buff;
  }
  
}
