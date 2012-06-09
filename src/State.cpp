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
    //display the instruction
    const char* instr_name = instr->to_string();
    fprintf(stderr, "\033[36m[decode] %s\033[0m\n", instr_name);
    delete[] instr_name;
    //process the instruction
    bool rv = instr->process(*this);
    //free the instruction
    delete instr;
    //and return
    return rv;
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
    uint32_t index = (-addr/4)-1;
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
          fprintf(stderr, "   %s: [%16s] %s\n", format_flag((Flag)i), format_datastate(flags[i].state), flags[i].value ? "true" : "false");
        }
        else {
          fprintf(stderr, "   %s: [%16s] ----\n", format_flag((Flag)i), format_datastate(flags[i].state));
        }
      }
    }
    fprintf(stderr, "Stack:\n");
    for(int i = 0; i < stack.size(); i++) {
      if(stack[i].isvirtual() || stack[i].state == DS_STACK_PTR) {
        fprintf(stderr, "  %08X: [%16s] %08x\n", (uint32_t)(-4*(i+1)), format_datastate(stack[i].state), stack[i].value);
      }
      else {
        fprintf(stderr, "  %08X: [%16s] --------\n", (uint32_t)(-4*(i+1)), format_datastate(stack[i].state));
      }
    }
    fprintf(stderr, "\n");
  }
  
  
  const char* format_register(Register r)
  {    
    switch(r) {
      case REG_EAX: return "%eax";
      case REG_ECX: return "%ecx";
      case REG_EDX: return "%edx";
      case REG_EBX: return "%ebx";
      case REG_ESP: return "%esp";
      case REG_EBP: return "%ebp";
      case REG_ESI: return "%esi";
      case REG_EDI: return "%edi";
      default:
        fprintf(stderr, "Error: Unrecognized register (%s:%d).\n", __FILE__, __LINE__);
        exit(1);
    }
  }
  
  const char* format_flag(Flag f)
  {
    switch(f) {
      case FLAG_C: return "/c";
      case FLAG_P: return "/p";
      case FLAG_A: return "/a";
      case FLAG_Z: return "/z";
      case FLAG_S: return "/s";
      case FLAG_O: return "/o";
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
