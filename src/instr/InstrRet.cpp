#include <stdlib.h>
#include <stdio.h>

#include "../Instr.hpp"
#include "../Data.hpp"
#include "../Home.hpp"
#include "../State.hpp"
#include "InstrRet.hpp"

namespace Dyncprop {

  InstrRet::InstrRet()
  {
    
  }
  
  InstrRet::~InstrRet()
  {
    
  }

  std::vector<Home> InstrRet::inputs() const
  {
    fprintf(stderr, "Error: Not implemented yet (%s:%d).\n", __FILE__, __LINE__);
    exit(1);
  }

  std::vector<Home> InstrRet::outputs() const
  {
    fprintf(stderr, "Error: Not implemented yet (%s:%d).\n", __FILE__, __LINE__);
    exit(1);
  }

  std::vector<uint8_t> InstrRet::opcode() const
  {
    std::vector<uint8_t> rv;
    rv.push_back(0xC3);
    return rv;
  }

  const Instr* InstrRet::cprop(Home input, Data value) const
  {
    return NULL;
  }

  const Instr* InstrRet::canonicalize() const
  {
    return NULL;
  }

  bool InstrRet::process(State& s) const
  {
    //verify that the stack pointer actually does point to the stack
    if(s.regs[REG_ESP].state != DS_STACK_PTR) {
      fprintf(stderr,"Error: On return, stack pointer did not point to stack (%s:%d)\n",__FILE__,__LINE__);
      s.dump();
      exit(1);
    }
    //do the memory read
    Data ret_addr = s.stack_at(s.regs[REG_ESP].value);
    //verify that the return address has the correct state
    if(ret_addr.state != DS_RET_ADDR) {
      fprintf(stderr,"Error: On return, stack pointer did not point to return address, but instead had state [%s] (%s:%d)\n",
              format_datastate(ret_addr.state),__FILE__,__LINE__);
      exit(1);
    }
    //verify that BP holds the correct value
    if(s.regs[REG_EBP].state != DS_RET_BP) {
      fprintf(stderr,"Error: On return, value of EPB wasn't restored (%s:%d)\n",__FILE__,__LINE__);
      s.dump();
      exit(1);
    }
    //verify that BX holds the correct value
    if(s.regs[REG_EBX].state != DS_RET_BX) {
      fprintf(stderr,"Error: On return, value of EPX wasn't restored (%s:%d)\n",__FILE__,__LINE__);
      s.dump();
      exit(1);
    }
    //verify that SI holds the correct value
    if(s.regs[REG_ESI].state != DS_RET_SI) {
      fprintf(stderr,"Error: On return, value of ESI wasn't restored (%s:%d)\n",__FILE__,__LINE__);
      s.dump();
      exit(1);
    }
    //verify that DI holds the correct value
    if(s.regs[REG_EDI].state != DS_RET_DI) {
      fprintf(stderr,"Error: On return, value of EDI wasn't restored (%s:%d)\n",__FILE__,__LINE__);
      s.dump();
      exit(1);
    }
    //if the return value is not real, we must realize it
    if(s.regs[REG_EAX].state == DS_VIRTUAL) {
      Home::HomeRegister(REG_EAX).realize(s);
    }
    //emit the return
    emit(s);
    //return from the function
    return true;
  }
  
  Instr* InstrRet::parse(const uint8_t* ip)
  {
    uint8_t opc = *ip;
    if(opc == 0xC3) {
      return new InstrRet();
    }
    return NULL;
  }
  
  const char* InstrRet::to_string() const
  {
    char* buf = new char[16];
    sprintf(buf, "%s", "RET");
    return buf;
  }
    
  
}
