#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "../Instr.hpp"
#include "../Data.hpp"
#include "../Home.hpp"
#include "../State.hpp"
#include "InstrJC.hpp"
#include "InstrJmp.hpp"

namespace Dyncprop {

  InstrJC::InstrJC(uint32_t cond_, int8_t imm_): cond(cond_), imm(imm_)
  {
        
  }

  InstrJC::~InstrJC()
  {
    
  }

  std::vector<Home> InstrJC::inputs() const
  {
    std::vector<Home> rv;
    switch(cond) {
      case 0:
      case 1:
        rv.push_back(Home::HomeFlag(FLAG_O));
        break;
      case 2:
      case 3:
        rv.push_back(Home::HomeFlag(FLAG_C));
        break;
      case 4:
      case 5:
        rv.push_back(Home::HomeFlag(FLAG_Z));
        break;
      case 6:
      case 7:
        rv.push_back(Home::HomeFlag(FLAG_C));
        rv.push_back(Home::HomeFlag(FLAG_Z));
        break;
      case 8:
      case 9:
        rv.push_back(Home::HomeFlag(FLAG_S));
        break;
      case 10:
      case 11:
        rv.push_back(Home::HomeFlag(FLAG_P));
        break;
      case 12:
      case 13:
        rv.push_back(Home::HomeFlag(FLAG_S));
        rv.push_back(Home::HomeFlag(FLAG_O));
        break;
      case 14:
      case 15:
        rv.push_back(Home::HomeFlag(FLAG_Z));
        rv.push_back(Home::HomeFlag(FLAG_S));
        rv.push_back(Home::HomeFlag(FLAG_O));
        break;
      default:
        fprintf(stderr, "Error: Invalid jump condition (%s:%d).\n", __FILE__, __LINE__);
        exit(1);
    }
    return rv;
  }

  std::vector<Home> InstrJC::outputs() const
  {
    fprintf(stderr, "Error: Not implemented yet (%s:%d).\n", __FILE__, __LINE__);
    exit(1);
  }

  std::vector<uint8_t> InstrJC::opcode() const
  {
    std::vector<uint8_t> rv;
    rv.push_back(0x70 + cond);
    rv.push_back((uint8_t)imm);
    return rv;
  }

  const Instr* InstrJC::cprop(Home input, Data value) const
  {
    return NULL;
  }
  
  std::vector<Data> InstrJC::emulate(std::vector<Data> ins) const
  {
    return Instr::emulate(ins);
  }
  
  const Instr* InstrJC::canonicalize() const
  {
    fprintf(stderr, "Error: Not implemented yet (%s:%d).\n", __FILE__, __LINE__);
    exit(1);
  }
  
  bool InstrJC::process(State& s) const
  {
    std::vector<Home> ins = inputs();
    std::vector<bool> infs;
    for(int i = 0; i < ins.size(); i++) {
      Data d = ins[i].get(s);
      if(!(d.isvirtual())) {
        State* s_nottaken = s.clone();
        State* s_taken = s.clone(); s_taken->ip += (int32_t)imm;
        fprintf(stderr, "\033[33m[jump]   Processing not-taken branch.\033[0m\n");
        while(s_nottaken->step() == 0);
        fprintf(stderr, "\033[33m[jump]   Processing taken branch.\033[0m\n");
        while(s_taken->step() == 0);
        s.emitbuf.push_back(0x70+cond);
        s.emitbuf.push_back(5);
        s.emitbuf.push_back(0xE9);
        int32_t jrv = s_taken->emitbuf.size();
        s.emitbuf.push_back((uint8_t)jrv); jrv >>= 8;
        s.emitbuf.push_back((uint8_t)jrv); jrv >>= 8;
        s.emitbuf.push_back((uint8_t)jrv); jrv >>= 8;
        s.emitbuf.push_back((uint8_t)jrv); jrv >>= 8;
        for(int i = 0; i < s_taken->emitbuf.size(); i++) {
          s.emitbuf.push_back(s_taken->emitbuf[i]);
        }
        for(int i = 0; i < s_nottaken->emitbuf.size(); i++) {
          s.emitbuf.push_back(s_nottaken->emitbuf[i]);
        }
        delete s_nottaken;
        delete s_taken;
        fprintf(stderr, "\033[33m[jump]   Joined branches.\033[0m\n");
        return true;
        /*
        s.realize_everything();
        s.emitbuf.push_back(0x70 + cond);
        s.emitbuf.push_back(6);
        uint32_t target = (uint32_t)s.ip;
        s.emitbuf.push_back(0x68);
        s.emitbuf.push_back((uint8_t)target); target >>= 8;
        s.emitbuf.push_back((uint8_t)target); target >>= 8;
        s.emitbuf.push_back((uint8_t)target); target >>= 8;
        s.emitbuf.push_back((uint8_t)target); target >>= 8;
        s.emitbuf.push_back(0xC3);
        s.ip += (int32_t)imm;
        return false;
        */        
      }
      infs.push_back(!!(d.value));
    }
    if(condition(infs)) {
      s.ip += (int32_t)imm;
    }
    return false;
  }
  
  bool InstrJC::condition(std::vector<bool> fs) const
  {
    switch(cond) {
      case 0:
        return fs[0];
      case 1:
        return !fs[0];
      case 2:
        return fs[0];
      case 3:
        return !fs[0];
      case 4:
        return !fs[0];
      case 5:
        return fs[0];
      case 6:
        return fs[0] && fs[1];
      case 7:
        return (!fs[0]) && (!fs[1]);
      case 8:
        return fs[0];
      case 9:
        return !fs[0];
      case 10:
        return fs[0];
      case 11:
        return !fs[0];
      case 12:
        return (fs[0] != fs[1]);
      case 13:
        return (fs[0] == fs[1]);
      case 14:
        return fs[0] || (fs[1] != fs[2]);
      case 15:
        return (!fs[0]) && (fs[1] == fs[2]);
      default:
        fprintf(stderr, "Error: Invalid jump condition (%s:%d).\n", __FILE__, __LINE__);
        exit(1);
    }
  }

  Instr* InstrJC::parse(const uint8_t* ip)
  {
    uint8_t opc = *ip;
    if((opc >= 0x70)&&(opc < 0x80)) {
      return new InstrJC((uint32_t)(opc - 0x70), (int8_t)readimm8(ip+1));
    }
    return NULL;
  }

  const char* InstrJC::to_string() const
  {
    const char* opname;
    switch(cond) {
      case 0: opname = "JO"; break;
      case 1: opname = "JNO"; break;
      case 2: opname = "JB"; break;
      case 3: opname = "JNB"; break;
      case 4: opname = "JZ"; break;
      case 5: opname = "JNZ"; break;
      case 6: opname = "JBE"; break;
      case 7: opname = "JA"; break;
      case 8: opname = "JS"; break;
      case 9: opname = "JNS"; break;
      case 10: opname = "JPE"; break;
      case 11: opname = "JPO"; break;
      case 12: opname = "JL"; break;
      case 13: opname = "JGE"; break;
      case 14: opname = "JLE"; break;
      case 15: opname = "JG"; break;
      default:
        fprintf(stderr, "Error: Invalid jump condition (%s:%d).\n", __FILE__, __LINE__);
        exit(1);
    }
    char* buf = new char[64];
    if(imm >= 0) {
      sprintf(buf, "%s <%%ip+0x%02x>", opname, (int32_t)imm);
    }
    else {
      sprintf(buf, "%s <%%ip-0x%02x>", opname, -(int32_t)imm);
    }
    return buf;
  }
  
}
