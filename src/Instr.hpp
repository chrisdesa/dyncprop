#pragma once

#include <stdint.h>
#include <vector>

#include "Data.hpp"
#include "Home.hpp"

namespace Dyncprop {

  class Home;
  class Data;
  class State;

  class Instr {    
  public:
    virtual std::vector<Home> inputs() const = 0;
    virtual std::vector<Home> outputs() const = 0;
    virtual std::vector<uint8_t> opcode() const = 0;
    virtual void emit(State& s) const;
    virtual std::vector<int32_t> run(std::vector<int32_t> ins) const;
    virtual std::vector<Data> emulate(std::vector<Data> ins) const;
    virtual const Instr* cprop(Home input, Data value) const = 0;
    virtual const Instr* canonicalize() const = 0;
    
    virtual bool process(State& s) const;
    
    virtual const char* to_string() const;
    
  public:
    virtual ~Instr();
    
  public:
    static Instr* parse(const uint8_t* ip);
  };
  
  class ModRM {
  public:
    ModRM(bool w_, bool d_, const uint8_t* ip);
    ~ModRM();
    
  private:
    ModRM() {}
    
  private:
    Home opd1() const;
    Home opd2() const;
    
  public:
    Home opd_src() const;
    Home opd_dst() const;
    std::vector<uint8_t> opcode_suffix() const;

  public:
    bool w; //width 0 - 1bit, 1 - 4bit
    bool d; //direction
  public:
    bool m; //operand in memory
    Register opd1_reg;
    int32_t opd1_offset;
    Register opd2_reg;
  };

  uint8_t readimm8(const uint8_t* ip);
  uint16_t readimm16(const uint8_t* ip);
  uint32_t readimm32(const uint8_t* ip);
  
  void writeimm8(uint8_t* ip, uint8_t value);
  void writeimm16(uint8_t* ip, uint16_t value);
  void writeimm32(uint8_t* ip, uint32_t value);
}
