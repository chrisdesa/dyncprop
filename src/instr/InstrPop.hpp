#pragma once

#include <stdint.h>
#include <vector>

#include "../Instr.hpp"

namespace Dyncprop {

  class Home;
  class Data;
  class State;

  /**
   * InstrPop
   **/
  
  class InstrPop: public Instr {    
  public:
    virtual std::vector<Home> inputs() const;
    virtual std::vector<Home> outputs() const;
    virtual std::vector<uint8_t> opcode() const;
    virtual Instr* cprop(Home input, Data value) const;
    virtual std::vector<Data> emulate(std::vector<Data> ins) const;

    virtual const char* to_string() const;
    
  public:
    virtual ~InstrPop();  
    
  private:
    Register opd;
    InstrPop(Register opd_);

  public:
    static Instr* parse(const uint8_t* ip);
  };

}
