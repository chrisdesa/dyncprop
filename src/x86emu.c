#include <stdlib.h>
#include <stdio.h>

#include "x86emu.h"

uint8_t readimm8(const uint8_t* ip);
uint16_t readimm16(const uint8_t* ip);
uint32_t readimm32(const uint8_t* ip);

//returns 0 normally, 1 if a return has occured
int x86step(x86state* ps)
{
  //get instruction pointer
  const uint8_t opcode = *(ps->ip++);
  x86opcodefamily* opcf = &(opc_families[opcode]);
  opcf->procfx(ps, opcf, opcode);
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
        pmodrm->opd1_address = ADDR_DISP;
      }
      else {
        //no displacement
        pmodrm->opd1_reg = REG_NONE;
        pmodrm->opd1_displacement = 0;
        pmodrm->opd1_address = (x86address)rm;
      }
      break;
    case 1:
      //8-bit displacement
      pmodrm->opd1_reg = REG_NONE;
      pmodrm->opd1_displacement = (int32_t)((int8_t)readimm8(ps->ip)); ps->ip += 1;
      pmodrm->opd1_address = (x86address)rm;
      break;
      
    case 2:
      //32-bit displacement
      pmodrm->opd1_reg = REG_NONE;
      pmodrm->opd1_displacement = (int32_t)readimm32(ps->ip); ps->ip += 4;
      pmodrm->opd1_address = (x86address)rm;
      break;
      
    case 3:
      //register operand
      pmodrm->opd1_reg = (x86register)rm;
      pmodrm->opd1_displacement = 0; //shouldn't be used
      pmodrm->opd1_address = ADDR_NONE;
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

x86data x86data_uninitialized()
{
  x86data rv;
  rv.state = DS_UNINITIALIZED;
  rv.value = 0;
  return rv;
}

x86data x86data_real(int32_t v)
{
  x86data rv;
  rv.state = DS_REAL;
  rv.value = v;
  return rv;
}

x86data x86data_symbolic()
{
  x86data rv;
  rv.state = DS_SYMBOLIC;
  rv.value = 0;
  return rv;
}

x86data x86data_stack_ptr(int32_t v)
{
  x86data rv;
  rv.state = DS_STACK_PTR;
  rv.value = v;
  return rv;
}

x86data x86data_ret_addr()
{
  x86data rv;
  rv.state = DS_RET_ADDR;
  rv.value = 0;
  return rv;
}
