#include <stdlib.h>
#include <stdio.h>

#include "Instr.hpp"
#include "Data.hpp"
#include "Home.hpp"

#include "Instructions.hpp"

namespace Dyncprop {
 
  Instr::~Instr()
  {
    
  }
  
  Instr* Instr::parse(const uint8_t* ip)
  {
    Instr* rv = NULL;
#define INSTR(inc) if((rv = inc::parse(ip))!=NULL) return rv;
#include "Instructions.list"
#undef INSTR
    return NULL;
  }
  
  void Instr::emit(State& s) const
  {
    std::vector<uint8_t> opc = opcode();
    for(int i = 0; i < opc.size(); i++) {
      s.emitbuf.push_back(opc[i]);
    }
  }
  
  std::vector<int32_t> Instr::run(std::vector<int32_t> ins) const
  {
    fprintf(stderr, "Error: Not implemented yet (%s:%d).\n", __FILE__, __LINE__);
    exit(1);
  }
  
  std::vector<Data> Instr::emulate(std::vector<Data> ins) const
  {
    uint32_t insz = inputs().size();
    uint32_t outsz = outputs().size();
    if(ins.size() != insz) {
      fprintf(stderr, "Error: Vector size mismatch (%s:%d).\n", __FILE__, __LINE__);
      exit(1);
    }
    std::vector<Data> rv;
    for(int i = 0; i < outsz; i++) {
      rv.push_back(data(DS_SYMBOLIC, 0));
    }
    return rv;
  }
  
  ModRM::ModRM(bool w_, bool d_, const uint8_t* ip): w(w_), d(d_)
  {
    const uint8_t opc = *(ip++);
    //split into mod, reg, and rm fields
    uint8_t mod = (opc >> 6) & 3 /*0b011*/;
    uint8_t reg = (opc >> 3) & 7 /*0b0111*/;
    uint8_t rm  = (opc >> 0) & 7 /*0b0111*/;
    //parse reg field (first operand)
    opd2_reg = (Register)reg;
    //look at mod field
    switch(mod) {
      case 0:
        if(rm == 6 /*0b110*/) {
          //displacement is address
          m = true;
          opd1_reg = REG_NONE;
          opd1_offset = (int32_t)readimm32(ip);
        }
        else {
          //no displacement
          m = true;
          opd1_offset = 0;
          opd1_reg = (Register)rm;
        }
        break;
      case 1:
        //8-bit displacement
        m = true;
        opd1_offset = (int32_t)((int8_t)readimm8(ip));
        opd1_reg = (Register)rm;
        break;
        
      case 2:
        //32-bit displacement
        m = true;
        opd1_offset = (int32_t)readimm32(ip);
        opd1_reg = (Register)rm;
        break;
        
      case 3:
        //register operand
        m = false;
        opd1_offset = 0; //shouldn't be used
        opd1_reg = (Register)rm;
        break;
    }
    //modrm field fully decoded
  }
  
  ModRM::~ModRM()
  {
    
  }
  
  Home ModRM::opd1() const
  {
    if(m) {
      if(opd1_reg == REG_NONE) {
        return Home::HomeMemoryConst(opd1_offset);
      }
      else {
        return Home::HomeMemory(opd1_reg, opd1_offset);
      }
    }
    else {
      return Home::HomeRegister(opd1_reg);
    }
  }
  
  Home ModRM::opd2() const
  {
    return Home::HomeRegister(opd2_reg);
  }
    
  Home ModRM::opd_dst() const
  {
    if(d) {
      return opd2();
    }
    else {
      return opd1();
    }
  }
  
  Home ModRM::opd_src() const
  {
    if(d) {
      return opd1();
    }
    else {
      return opd2();
    }
  }
  
  std::vector<uint8_t> ModRM::opcode_suffix() const
  {
    std::vector<uint8_t> rv;
    if(m) {
      if(opd1_reg == REG_NONE) {
        uint8_t modrm = (0 << 6) | (opd2_reg << 3) | 6;
        rv.push_back(modrm);
        uint32_t imm = opd1_offset;
        rv.push_back((uint8_t)(imm & 255)); imm >>= 8;
        rv.push_back((uint8_t)(imm & 255)); imm >>= 8;
        rv.push_back((uint8_t)(imm & 255)); imm >>= 8;
        rv.push_back((uint8_t)(imm & 255)); imm >>= 8;
      }
      else {
        if((opd1_offset == 0)&&(opd1_reg != 6)) {
          uint8_t modrm = (0 << 6) | (opd2_reg << 3) | (opd1_reg);
          rv.push_back(modrm);
        }
        else if((opd1_offset >= -128)||(opd1_offset < 128)) {
          uint8_t modrm = (1 << 6) | (opd2_reg << 3) | (opd1_reg);
          rv.push_back(modrm);
          rv.push_back((uint8_t)opd1_offset);
        }
        else {
          uint8_t modrm = (2 << 6) | (opd2_reg << 3) | (opd1_reg);
          rv.push_back(modrm);
          uint32_t imm = opd1_offset;
          rv.push_back((uint8_t)(imm & 255)); imm >>= 8;
          rv.push_back((uint8_t)(imm & 255)); imm >>= 8;
          rv.push_back((uint8_t)(imm & 255)); imm >>= 8;
          rv.push_back((uint8_t)(imm & 255)); imm >>= 8;
        }
      }
    }
    else {
      uint8_t modrm = (3 << 6) | (opd2_reg << 3) | (opd1_reg);
      rv.push_back(modrm);
    }
    return rv;
  }
  
  uint8_t readimm8(const uint8_t* ip)
  {
    return ip[0];
  }

  uint16_t readimm16(const uint8_t* ip)
  {
    return ((uint16_t)ip[0]) + ((uint16_t)ip[1]) << 8;
  }

  uint32_t readimm32(const uint8_t* ip)
  {
    return ((uint16_t)ip[0]) + ((uint16_t)ip[1]) << 8
        + ((uint16_t)ip[2] << 16) + ((uint16_t)ip[3]) << 24;
  }
  
}
