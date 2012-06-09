#include <stdlib.h>
#include <stdio.h>

#include "State.hpp"
#include "Home.hpp"
#include "Instr.hpp"
#include "Data.hpp"

namespace Dyncprop {
  
  State::State()
  {
    
  }
  
  bool State::step()
  {
    //parse the next instruction
    Instr* instr = Instr::parse(ip);
    //check if it was properly parsed
    if(instr == NULL) {
      fprintf(stderr, "Error: Unrecognized instruction %02x %02x %02x %02x ...\n", (uint32_t)ip[0], (uint32_t)ip[1], (uint32_t)ip[2], (uint32_t)ip[3]);
      dump();
      exit(1);
    }
    //increment the instruction pointer
    ip += instr->opcode().size();
    //get the inputs and outputs
    std::vector<Home> inputs = instr->inputs();
    std::vector<Home> outputs = instr->outputs();
    //check if all the inputs are virtual
    bool allvirtual = true;
    for(int i = 0; i < inputs.size(); i++) {
      allvirtual = allvirtual && (inputs[i].get(*this).isvirtual());
    }
    //if all the inputs are virtual, run the instruction
    if(allvirtual) {
      //assemble a vector of the input values
      std::vector<int32_t> in_vs;
      for(int i = 0; i < inputs.size(); i++) {
        in_vs.push_back(inputs[i].get(*this).value);
      }
      //run the instructions
      std::vector<int32_t> out_vs = instr->run(in_vs);
      if(outputs.size() != out_vs.size()) {
        fprintf(stderr, "Error: Vector mismatch.\n");
        exit(1);
      }
      //get the output locations
      //we need to do this because the output locations may change as we write out
      std::vector<Data*> out_locs;
      for(int i = 0; i < outputs.size(); i++) {
        out_locs.push_back(&(outputs[i].access(*this)));
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
        Data nd = inputs[i].get(*this);
        if(nd.isvirtual()&&(!nd.issymbolic())) {
          Instr* ni = instr->cprop(inputs[i], nd);
          if(ni != NULL) {
            //restart the loop with the mutated instruction
            delete instr;
            instr = ni;
            inputs = instr->inputs();
            outputs = instr->outputs();
            i = 0;
            continue;
          }
          else {
            //realize the operand
            inputs[i].realize(*this);
          }
        }
      }
      //emit the instruction
      instr->emit(*this);
      //get the operand states
      std::vector<Data> in_ds;
      for(int i = 0; i < inputs.size(); i++) {
        in_ds.push_back(inputs[i].get(*this));
      }
      //emulate the instruction
      std::vector<Data> out_ds = instr->emulate(in_ds);
      //get the output locations
      //we need to do this because the output locations may change as we write out
      std::vector<Data*> out_locs;
      for(int i = 0; i < outputs.size(); i++) {
        out_locs.push_back(&(outputs[i].access(*this)));
      }
      //write the outputs
      for(int i = 0; i < outputs.size(); i++) {
        *(out_locs[i]) = out_ds[i];
      }
      //we're done processing the instruction
    }
    delete instr;
  }
  
  Data& State::stack_at(int32_t addr)
  {
    //make sure this address isn't above the stack
    if(addr >= 0) {
      fprintf(stderr, "Error: Stack access at address [%d] out-of-bounds (%s:%d).\n", addr, __FILE__, __LINE__);
      exit(1);
    }
    //check alignment
    if((addr & 3) != 0) {
      fprintf(stderr, "Error: Tried to access 32-bit value to stack at unaligned address (%s:%d).\n", __FILE__, __LINE__);
      exit(1);
    }
    //get index
    uint32_t index = 1+(-addr/4);
    //make sure stack hasn't grown too large
    if((index >= STACK_MAX)||(index >= stack.max_size())) {
      fprintf(stderr, "Error: Stack access at index [%d] exceeds maximum stack size (%s:%d).\n", index, __FILE__, __LINE__);
      dump();
      exit(1);
    }
    //potentially expand the stack
    if(index >= stack.size()) {
      stack.resize(index+1);
    }
    //return the address
    return stack[index];
  }
  
  void State::dump()
  {
    fprintf(stderr, "\n");
    fprintf(stderr, "Registers:\n");
    for(int i = 0; i < REG_ENUM_MAX; i++) {
      if(regs[i].isvirtual() || regs[i].state == DS_STACK_PTR) {
        fprintf(stderr, "  %s: [%16s] %08x\n", format_register((Register)i), format_datastate(regs[i].state), regs[i].value);
      }
      else {
        fprintf(stderr, "  %s: [%16s] --------\n", format_register((Register)i), format_datastate(regs[i].state));
      }
    }
    fprintf(stderr, "Flags:\n");
    for(int i = 0; i < FLAG_ENUM_MAX; i++) {
      if(flag_valid((Flag)i)) {
        if(flags[i].isvirtual()) {
          fprintf(stderr, "    %s: [%16s] %s\n", format_flag((Flag)i), format_datastate(flags[i].state), flags[i].value ? "true" : "false");
        }
        else {
          fprintf(stderr, "    %s: [%16s] ----\n", format_flag((Flag)i), format_datastate(flags[i].state));
        }
      }
    }
    fprintf(stderr, "Stack:\n");
    for(int i = 0; i < stack.size(); i++) {
      if(stack[i].isvirtual() || stack[i].state == DS_STACK_PTR) {
        fprintf(stderr, "  %08X: [%16s] %08x\n", (uint32_t)(-4*i), format_datastate(stack[i].state), stack[i].value);
      }
      else {
        fprintf(stderr, "  %08X: [%16s] --------\n", (uint32_t)(-4*i), format_datastate(stack[i].state));
      }
    }
    fprintf(stderr, "\n");
  }
  
  
  const char* format_register(Register r)
  {    
    switch(r) {
#define FMT_CASE(x) case x: return #x;
    FMT_CASE(REG_EAX)
    FMT_CASE(REG_ECX)
    FMT_CASE(REG_EDX)
    FMT_CASE(REG_EBX)
    FMT_CASE(REG_ESP)
    FMT_CASE(REG_EBP)
    FMT_CASE(REG_ESI)
    FMT_CASE(REG_EDI)
    FMT_CASE(REG_NONE)
#undef FMT_CASE
      default:
        fprintf(stderr, "Error: Unrecognized register (%s:%d).\n", __FILE__, __LINE__);
        exit(1);
    }
  }
  
  const char* format_flag(Flag f)
  {
    switch(f) {
#define FMT_CASE(x) case x: return #x;
    FMT_CASE(FLAG_C)
    FMT_CASE(FLAG_P)
    FMT_CASE(FLAG_A)
    FMT_CASE(FLAG_Z)
    FMT_CASE(FLAG_S)
    FMT_CASE(FLAG_O)
#undef FMT_CASE
      default:
        fprintf(stderr, "Error: Unrecognized flag (%s:%d).\n", __FILE__, __LINE__);
        exit(1);
    }
  }
  
  bool register_valid(Register r)
  {
    return (r >= 0)&&(r < REG_ENUM_MAX);
  }
  
  bool flag_valid(Flag f)
  {
    switch(f) {
      case FLAG_C:
      case FLAG_P:
      case FLAG_A:
      case FLAG_Z:
      case FLAG_S:
      case FLAG_O:
        return true;
      default:
        return false;
    }
  }
  
}
