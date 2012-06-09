#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <vector>

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
    //display the instruction
    const char* instr_name = this->to_string();
    fprintf(stderr, "\033[35m[emit]   %s\t\t", instr_name);
    delete[] instr_name;
    std::vector<uint8_t> opc = opcode();
    for(int i = 0; i < opc.size(); i++) {
      fprintf(stderr, "%02x ", (uint32_t)opc[i]);
      s.emitbuf.push_back(opc[i]);
    }
    fprintf(stderr, "\033[0m\n");
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
  
  bool Instr::process(State& s) const
  {
    //get the inputs and outputs
    std::vector<Home> inputs = this->inputs();
    std::vector<Home> outputs = this->outputs();
    //check if all the inputs are virtual
    bool allvirtual = true;
    for(int i = 0; i < inputs.size(); i++) {
      allvirtual = allvirtual && (inputs[i].get(s).isvirtual());
    }
    //if all the inputs are virtual, run the instruction
    if(allvirtual) {
      //assemble a vector of the input values
      std::vector<int32_t> in_vs;
      for(int i = 0; i < inputs.size(); i++) {
        in_vs.push_back(inputs[i].get(s).value);
      }
      //run the instructions
      std::vector<int32_t> out_vs = run(in_vs);
      if(outputs.size() != out_vs.size()) {
        fprintf(stderr, "Error: Vector mismatch [%d vs %d] (%s:%d).\n", outputs.size(), out_vs.size(), __FILE__, __LINE__);
        exit(1);
      }
      //get the output locations
      //we need to do this because the output locations may change as we write out
      std::vector<Data*> out_locs;
      for(int i = 0; i < outputs.size(); i++) {
        out_locs.push_back(&(outputs[i].access(s)));
      }
      //write the outputs
      for(int i = 0; i < outputs.size(); i++) {
        *(out_locs[i]) = data(DS_VIRTUAL, out_vs[i]);
      }
      //we're done processing the instruction; f
    }
    else {
      //realize all the inputs
      for(int i = 0; i < inputs.size(); i++) {
        Data nd = inputs[i].get(s);
        if(nd.isvirtual()&&(!nd.issymbolic())) {
          Instr* ni = cprop(inputs[i], nd);
          if(ni != NULL) {
            //restart the function with the mutated instruction
            bool rv = ni->process(s);
            delete ni;
            return rv;
          }
          else {
            //realize the operand
            inputs[i].realize(s);
          }
        }
      }
      //emit the instruction
      emit(s);
      //get the operand states
      std::vector<Data> in_ds;
      for(int i = 0; i < inputs.size(); i++) {
        in_ds.push_back(inputs[i].get(s));
      }
      //emulate the instruction
      std::vector<Data> out_ds = emulate(in_ds);
      //get the output locations
      //we need to do this because the output locations may change as we write out
      std::vector<Data*> out_locs;
      for(int i = 0; i < outputs.size(); i++) {
        out_locs.push_back(&(outputs[i].access(s)));
      }
      //write the outputs
      for(int i = 0; i < outputs.size(); i++) {
        *(out_locs[i]) = out_ds[i];
      }
      //we're done processing the instruction
    }
    return false;
  }
  
  const char* Instr::to_string() const
  {
    char* buf = new char[16];
    strcpy(buf, "INSTR ?? ??");
    return buf;
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
