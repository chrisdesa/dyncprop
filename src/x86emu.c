#include <stdlib.h>
#include <stdio.h>

#include "x86emu.h"

uint8_t readimm8(const uint8_t* ip);
uint16_t readimm16(const uint8_t* ip);
uint32_t readimm32(const uint8_t* ip);

//returns 0 normally, 1 if a return has occured
int x86step(x86state* ps)
{
  //get opcode family
  const uint8_t opcode = *(ps->ip++);
  x86opcodefamily* opcf = &(opc_families[opcode]);
  //call processing function for that opcode family
  return opcf->procfx(ps, opcf, opcode);
}

void x86proc_modrm(x86state* ps, x86modrm* pmodrm)
{
  const uint8_t opc = *(ps->ip++);
  //split into mod, reg, and rm fields
  uint8_t mod = (opc >> 6) & 3 /*0b011*/;
  uint8_t reg = (opc >> 3) & 7 /*0b0111*/;
  uint8_t rm  = (opc >> 0) & 7 /*0b0111*/;
  //parse reg field (first operand)
  pmodrm->opd2 = (x86register)reg;
  //look at mod field
  switch(mod) {
    case 0:
      if(rm == 6 /*0b110*/) {
        //displacement is address
        pmodrm->opd1_reg = REG_NONE;
        pmodrm->opd1_displacement = (int32_t)readimm32(ps->ip); ps->ip += 4;
        pmodrm->opd1_address = REG_NONE;
      }
      else {
        //no displacement
        pmodrm->opd1_reg = REG_NONE;
        pmodrm->opd1_displacement = 0;
        pmodrm->opd1_address = (x86register)rm;
      }
      break;
    case 1:
      //8-bit displacement
      pmodrm->opd1_reg = REG_NONE;
      pmodrm->opd1_displacement = (int32_t)((int8_t)readimm8(ps->ip)); ps->ip += 1;
      pmodrm->opd1_address = (x86register)rm;
      break;
      
    case 2:
      //32-bit displacement
      pmodrm->opd1_reg = REG_NONE;
      pmodrm->opd1_displacement = (int32_t)readimm32(ps->ip); ps->ip += 4;
      pmodrm->opd1_address = (x86register)rm;
      break;
      
    case 3:
      //register operand
      pmodrm->opd1_reg = (x86register)rm;
      pmodrm->opd1_displacement = 0; //shouldn't be used
      pmodrm->opd1_address = REG_NONE;
      break;
  }
  //modrm field fully decoded
}

void x86emit(x86state* ps, const uint8_t* instr, uint32_t len)
{
  int i;
  while(ps->emitidx + len > ps->emitsz) {
    //expand emission buffer
    ps->emitsz *= 2;
    ps->emitbuf = realloc(ps->emitbuf, ps->emitsz);
  }
  for(i = 0; i < len; i++) {
    ps->emitbuf[ps->emitidx + i] = instr[i];
  }
  ps->emitidx += len;
  if(ps->emitidx > EMIT_MAX) {
    fprintf(stderr, "Error: Emitted instructions exceeeded emission maximum.\n");
    exit(1);
  }
  
  fprintf(stderr, "\033[36m[emit]\033[0m:");
  for(i = 0; i < len; i++) {
    fprintf(stderr, " %02X", instr[i]);
  }
  fprintf(stderr, "\n");
}

void x86emit1(x86state* ps, uint8_t instr)
{
  x86emit(ps, &instr, 1);
}

uint8_t readimm8(const uint8_t* ip)
{
  return ip[0];
}

uint16_t readimm16(const uint8_t* ip)
{
  return ((uint16_t)ip[0]) + ((uint16_t)ip[1]) << 8;
}

uint32_t readimm32(const uint8_t* ip)
{
  return ((uint16_t)ip[0]) + ((uint16_t)ip[1]) << 8
       + ((uint16_t)ip[2] << 16) + ((uint16_t)ip[3]) << 24;
}


x86data x86data_init(x86datastate state, int32_t value)
{  
  x86data rv;
  rv.state = state;
  rv.value = value;
  return rv;
}

//the reason we need to have this function is that the x86 stack
//grows from high addresses to low addresses, while the memory that
//we use to store our virtualized stack grows from low to high.  So,
//we have to reverse the order in which (32-bit) words are stored in memory
void x86stack_write32(x86state* ps, int32_t addr, x86data value)
{
  *x86stack_access32(ps, addr) = value;
}

x86data x86stack_read32(x86state* ps, int32_t addr)
{
  return *x86stack_access32(ps, addr);
}

x86data* x86stack_access32(x86state* ps, int32_t addr)
{
  //make sure this address isn't above the stack
  if(addr >= 0) {
    fprintf(stderr, "Error: Stack write out-of-bounds.\n");
    exit(1);
  }
  //check alignment
  if((addr & 3) != 0) {
    fprintf(stderr, "Error: Tried to write 32-bit value to stack at unaligned address.\n");
    exit(1);
  }
  //get index
  uint32_t index = 1+(-addr/4);
  //make sure stack hasn't grown too large
  if(index >= STACK_MAX) {
    fprintf(stderr, "Error: Stack write exceeds maximum stack size.\n");
    exit(1);
  }    
  //potentially expand the stack
  while(index >= ps->stacksz) {
    ps->pstack = realloc(ps->pstack, sizeof(x86data) * ps->stacksz * 2);
    for(int i = 0; i < ps->stacksz; i++) {
      ps->pstack[i + ps->stacksz] = x86data_init(DS_UNINITIALIZED, 0); 
    }
    ps->stacksz *= 2;
  }
  //return the address
  return &(ps->pstack[index]);
}

int issymbolic(x86datastate ds)
{
  return (ds == DS_SYMBOLIC)||(ds == DS_RET_ADDR)||(ds == DS_RET_BP)||(ds == DS_STACK_PTR);
}

int isreal(x86datastate ds)
{
  return (ds == DS_UNINITIALIZED)||(ds == DS_REAL);
}


x86data x86make_address(x86state* ps, x86register addr, int32_t offset)
{
  x86data rv;
  if(addr == REG_NONE) {
    rv.state = DS_REAL;
    rv.value = offset;
  }
  else {
    switch(ps->regs[addr].state) {
      case DS_UNINITIALIZED:
        fprintf(stderr, "Error: Read from uninitialized address (%s:%d).\n",__FILE__,__LINE__);
        exit(1);
      case DS_REAL:
        rv.state = DS_REAL;
        rv.value = ps->regs[addr].value + offset;
        break;
      case DS_SYMBOLIC:
        rv.state = DS_SYMBOLIC;
        rv.value = 0;
        break;
      case DS_STACK_PTR:
        rv.state = DS_STACK_PTR;
        rv.value = ps->regs[addr].value + offset;
        break;
      default:
        fprintf(stderr, "Error: Invalid address state [%s] (%s:%d).\n",x86datastate_tostr(ps->regs[addr].state),__FILE__,__LINE__);
        exit(1);
    }
  }
  return rv;
}


#define case_to_str(x) case x: return #x ;

const char* x86datastate_tostr(x86datastate ds)
{
  switch(ds) {
    case_to_str(DS_UNINITIALIZED)
    case_to_str(DS_REAL)
    case_to_str(DS_SYMBOLIC)
    case_to_str(DS_STACK_PTR)
    case_to_str(DS_RET_ADDR)
    case_to_str(DS_RET_BP)
    default:
      fprintf(stderr, "Error: Invalid state (%s:%d).\n",__FILE__,__LINE__);
      exit(1);
  }
}

const char* x86register_tostr(x86register reg)
{
  switch(reg) {
    case_to_str(REG_EAX)
    case_to_str(REG_ECX)
    case_to_str(REG_EDX)
    case_to_str(REG_EBX)
    case_to_str(REG_ESP)
    case_to_str(REG_EBP)
    case_to_str(REG_ESI)
    case_to_str(REG_EDI)
    case_to_str(REG_NONE)
    default:
      fprintf(stderr, "Error: Invalid register (%s:%d).\n",__FILE__,__LINE__);
      exit(1);
  }
}