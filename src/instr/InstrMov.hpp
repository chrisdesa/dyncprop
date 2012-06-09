#pragma once

#include <stdint.h>
#include <vector>

#include "../Instr.hpp"

namespace Dyncprop {

  class Home;
  class Data;
  class State;

  /**
   * InstrMov
   **/
  
  class InstrMov: public Instr {
  public:
    InstrMov(bool w_, bool d_, const uint8_t* ip);
    
  private:
    ModRM modrm;
    
  public:
    virtual std::vector<Home> inputs() const;
    virtual std::vector<Home> outputs() const;
    virtual std::vector<uint8_t> opcode() const;
    virtual Instr* cprop(Home input, Data value) const;
    virtual std::vector<Data> emulate(std::vector<Data> ins) const;
    virtual std::vector<int32_t> run(std::vector<int32_t> ins) const;

    virtual const char* to_string() const;
    
  public:
    virtual ~InstrMov();    

  public:
    static Instr* parse(const uint8_t* ip);
  };

}
