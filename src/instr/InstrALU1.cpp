#include <stdlib.h>
#include <stdio.h>

#include "../Instr.hpp"
#include "../Data.hpp"
#include "../Home.hpp"
#include "../State.hpp"
#include "InstrALU1.hpp"
#include "InstrGroup1.hpp"
#include "InstrALU1AI.hpp"

namespace Dyncprop {

  InstrALU1::InstrALU1(uint32_t alu_opid, bool w, bool d, const uint8_t* ip): opid(alu_opid), modrm(w,d,ip)
  {
    
  }
  
  InstrALU1::~InstrALU1()
  {
    
  }
  
  std::vector<Home> InstrALU1::inputs() const
  {
    std::vector<Home> rv;
    rv.push_back(modrm.opd_dst());
    rv.push_back(modrm.opd_src());
    if((opid == 2)||(opid == 3)) {
      //these ops depend on the carry flag
      rv.push_back(Home::HomeFlag(FLAG_C));
    }
    return rv;
  }

  std::vector<Home> InstrALU1::outputs() const
  {
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
  
  std::vector<Data> InstrALU1::emulate(std::vector<Data> ins) const
  {
    std::vector<Data> outs = Instr::emulate(ins);
    switch(opid) {
      case 0: //ADD
        if((ins[0].state == DS_STACK_PTR)&&(ins[1].isvirtual())) {
          outs[0] = data(DS_STACK_PTR, ins[0].value + ins[1].value);
        }
        else if((ins[1].state == DS_STACK_PTR)&&(ins[0].isvirtual())) {
          outs[0] = data(DS_STACK_PTR, ins[0].value + ins[1].value);
        }
        break;
      case 5: //SUB
        if((ins[0].state == DS_STACK_PTR)&&(ins[1].isvirtual())) {
          outs[0] = data(DS_STACK_PTR, ins[0].value - ins[1].value);
        }
        break;
      //we could allow ADC and SBB, but that's probably overkill
    }
    return outs;
  }
  /*
  std::vector<int32_t> InstrALU1::run(std::vector<int32_t> ins) const
  {
    if(opid == 0) {
      fprintf(stderr, "Warning: Addition flags not implemented yet (%s:%d).\n", __FILE__, __LINE__);
      std::vector<int32_t> rv;
      rv.push_back(ins[0] + ins[1]);
      rv.push_back(0);
      rv.push_back(0);
      rv.push_back(0);
      rv.push_back(0);
      rv.push_back(0);
      rv.push_back(0);
      return rv;
    }
    else {
      fprintf(stderr, "Error: ALU ops other than add not implemented yet (%s:%d).\n", __FILE__, __LINE__);
      exit(1);
    }
  }
  */
  std::vector<uint8_t> InstrALU1::opcode() const
  {
    std::vector<uint8_t> rv;
    uint8_t opc = (opid << 3) | (modrm.w ? 1 : 0) | (modrm.d ? 2 : 0);
    rv.push_back(opc);
    std::vector<uint8_t> suffix = modrm.opcode_suffix();
    for(int i = 0; i < suffix.size(); i++) {
      rv.push_back(suffix[i]);
    }
    return rv;
  }

  const Instr* InstrALU1::cprop(Home input, Data value) const
  {
    if(value.isvirtual()) {
      if(input == modrm.opd_src()) {
        std::vector<uint8_t> modrm_suffix = modrm.opcode_suffix();
        modrm_suffix[0] = (modrm_suffix[0] & (~(7<<3))) | opid << 3;
        return new InstrGroup1(modrm.w, &(modrm_suffix[0]), value.value);
      }
    }
    return NULL;
  }
  
  const Instr* InstrALU1::canonicalize() const
  {
    uint8_t v_modrm = (3 << 6)|(1 << 3)|(2);
    return new InstrALU1(opid, modrm.w, modrm.d, &v_modrm);
  }

  Instr* InstrALU1::parse(const uint8_t* ip)
  {
    uint8_t opc = *ip;
    if((!(opc & 4))&&(opc >= 0x00)&&(opc < 0x40)) {
      return new InstrALU1((opc >> 3) & 7, opc & 1, (opc >> 1) & 1, ip+1);
    }
    return NULL;
  }
  
  const char* InstrALU1::to_string() const
  {
    const char* opid_name = opid_to_string(opid);
    const char* opddst_name = modrm.opd_dst().to_string();
    const char* opdsrc_name = modrm.opd_src().to_string();
    char* buf = new char[256];
    sprintf(buf, "%s %s, %s", opid_name, opddst_name, opdsrc_name);
    delete[] opddst_name;
    delete[] opdsrc_name;
    return buf;
  }
  
  const char* InstrALU1::opid_to_string(uint32_t opid)
  {    
    switch(opid) {
      case 0:
        return "ADD";
      case 1:
        return "OR";
      case 2:
        return "ADC";
      case 3:
        return "SBB";
      case 4:
        return "AND";
      case 5:
        return "SUB";
      case 6:
        return "XOR";
      case 7:
        return "CMP";
      default:
        fprintf(stderr, "Error: Invalid ALU op (%s:%d).\n", __FILE__, __LINE__);
        exit(1);
    }
  }
}
