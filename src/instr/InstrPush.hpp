#pragma once

#include <stdint.h>
#include <vector>

#include "../State.hpp"
#include "../Instr.hpp"

namespace Dyncprop {

  class Home;
  class Data;
  class State;

  /**
   * InstrPush
   * 
   * Contains instructions:
   *   PUSH $reg
   **/
  
  class InstrPush: public Instr {
  public:
    virtual std::vector<Home> inputs() const;
    virtual std::vector<Home> outputs() const;
    virtual std::vector<uint8_t> opcode() const;
    virtual Instr* cprop(Home input, Data value) const;
    virtual std::vector<Data> emulate(std::vector<Data> ins) const;
    
  public:
    virtual ~InstrPush();
    
  private:
    Register opd;
    InstrPush(Register opd_);
    
  public:
    static Instr* parse(const uint8_t* ip);
  };

}
