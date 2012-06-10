#include <stdlib.h>
#include <stdio.h>

#include "Home.hpp"
#include "State.hpp"
#include "Data.hpp"
#include "instr/InstrMovi.hpp"
#include "instr/InstrMovImm.hpp"

namespace Dyncprop {
  
  Home::Home()
  {
    
  }
  
  Home::Home(const Home& h)
  {
    mode = h.mode;
    r = h.r;
    f = h.f;
    offset = h.offset;
  }
  
  Home& Home::operator =(const Home& h)
  {
    mode = h.mode;
    r = h.r;
    f = h.f;
    offset = h.offset;
  }
  
  bool Home::operator ==(const Home& h) const
  {
    if(mode != h.mode) return false;
    switch(mode) {
      case HM_REGISTER:
        return (r == h.r);
      case HM_FLAG:
        return (f == h.f);
      case HM_MEMORY:
        return (r == h.r)&&(offset == h.offset);
      case HM_MEMORY_CONST:
        return (offset == h.offset);
      default:
        fprintf(stderr, "Error: Invalid home mode (%s:%d).\n", __FILE__, __LINE__);
        exit(1);
    }
  }
  
  bool Home::operator !=(const Home& h) const
  {
    return !((*this) == h);
  }
  
  void Home::realize(State& s) const
  {
    Data& data = access(s);
    if(data.state != DS_VIRTUAL) {
      fprintf(stderr, "Error: Attempted to realize nonvirtual data (%s:%d).\n", __FILE__, __LINE__);
      exit(1);
    }
    switch(mode) {
      case HM_REGISTER:
        {
          Instr* instr = new InstrMovi(r, data.value);
          instr->emit(s);
          delete instr;
          data.state = DS_REAL;
        }
        break;
      case HM_FLAG:
        fprintf(stderr, "Error: Realizing flags not supported (%s:%d).\n", __FILE__, __LINE__);
        exit(1);
      case HM_MEMORY:
        {
          Instr* instr = new InstrMovImm(r, offset, data.value);
          instr->emit(s);
          delete instr;
          data.state = DS_REAL;
        }
        break;
      case HM_MEMORY_CONST:
        fprintf(stderr, "Error: Realizing constant memory not supported (%s:%d).\n", __FILE__, __LINE__);
        exit(1);
      default:
        fprintf(stderr, "Error: Invalid home mode (%s:%d).\n", __FILE__, __LINE__);
        exit(1);
    }
  }
  
  Data Home::get(State& s) const
  {
    switch(mode) {
      case HM_REGISTER:
        return s.regs[r];
      case HM_FLAG:
        return s.flags[f];
      case HM_MEMORY:
      {
        Data dr = s.regs[r];
        uint32_t addr = dr.value + offset;
        switch(dr.state) {
          case DS_VIRTUAL:
          case DS_REAL:
            return data(DS_VIRTUAL, *(int32_t*)addr);
            
          case DS_STACK_PTR:
            return s.stack_at(addr);
            
          default:
            fprintf(stderr, "Error: Invalid register state [%s] on memory read (%s:%d).\n", format_datastate(dr.state), __FILE__, __LINE__);
            s.dump();
            exit(1);
        }
      }
      case HM_MEMORY_CONST:
        return data(DS_VIRTUAL, *(int32_t*)offset);
      default:
        fprintf(stderr, "Error: Invalid home mode (%s:%d).\n", __FILE__, __LINE__);
        exit(1);
    }
  }
  
  
  Data& Home::access(State& s) const
  {
    switch(mode) {
      case HM_REGISTER:
        return s.regs[r];
      case HM_FLAG:
        return s.flags[f];
      case HM_MEMORY:
        {
          Data dr = s.regs[r];
          uint32_t addr = dr.value + offset;
          switch(dr.state) {
            case DS_STACK_PTR:
              return s.stack_at(addr);
            default:
              fprintf(stderr, "Error: Invalid register state [%s] on memory access (%s:%d).\n", format_datastate(dr.state), __FILE__, __LINE__);
              s.dump();
              exit(1);
          }
        }
        break;
      case HM_MEMORY_CONST:
        fprintf(stderr, "Error: Invalid memory mode (%s:%d).\n", __FILE__, __LINE__);
        exit(1);
      default:
        fprintf(stderr, "Error: Invalid home mode (%s:%d).\n", __FILE__, __LINE__);
        exit(1);
    }
  }
  
  Home Home::HomeRegister(Register r)
  {
    if((r < 0)||(r >= REG_ENUM_MAX)) {
      fprintf(stderr, "Error: Invalid register (%s:%d).\n", __FILE__, __LINE__);
      exit(1);
    }
    Home rv;
    rv.mode = HM_REGISTER;
    rv.r = r;
    return rv;
  }
  
  Home Home::HomeFlag(Flag f)
  {
    switch(f) {
      case FLAG_C:
      case FLAG_P:
      case FLAG_A:
      case FLAG_Z:
      case FLAG_S:
      case FLAG_O:
        break;
      default:
        fprintf(stderr, "Error: Invalid flag (%s:%d).\n", __FILE__, __LINE__);
        exit(1);
    }
    Home rv;
    rv.mode = HM_FLAG;
    rv.f = f;
    return rv;
  }
  
  Home Home::HomeMemory(Register r, int32_t offset)
  {    
    if((r < 0)||(r >= REG_ENUM_MAX)) {
      fprintf(stderr, "Error: Invalid register (%s:%d).\n", __FILE__, __LINE__);
      exit(1);
    }
    Home rv;
    rv.mode = HM_MEMORY;
    rv.r = r;
    rv.offset = offset;
    return rv;
  }
  
  Home Home::HomeMemoryConst(int32_t offset)
  {
    Home rv;
    rv.mode = HM_MEMORY_CONST;
    rv.offset = offset;
    return rv;
  }
  
  const char* Home::to_string() const
  {
    char* buf = new char[64];
    switch(mode) {
      case HM_REGISTER:
        sprintf(buf, "%s", format_register(r));
        break;
      case HM_FLAG:
        sprintf(buf, "%s", format_flag(f));
        break;
      case HM_MEMORY:
        if(offset >= 0) {
          sprintf(buf, "*(%s+%08x)", format_register(r), offset);
        }
        else if(offset < 0) {
          sprintf(buf, "*(%s-%08x)", format_register(r), -offset);
        }
        break;
      case HM_MEMORY_CONST:
        sprintf(buf, "*(%08x)", offset);
        break;
      default:
        fprintf(stderr, "Error: Invalid home mode (%s:%d).\n", __FILE__, __LINE__);
        exit(1);
    }
    return buf;
  }  
}