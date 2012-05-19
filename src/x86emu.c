#include <stdlib.h>
#include <stdio.h>

#include "x86emu.h"

uint8_t readimm8(const uint8_t* ip);
uint16_t readimm16(const uint8_t* ip);
uint32_t readimm32(const uint8_t* ip);

void x86step(x86state* ps)
{
  //get instruction pointer
  const uint8_t* ip = (const uint8_t*)ps->ip;
  const uint8_t opcode1 = *(ip++);
  switch(opcode1) {
    case 0x00: {
      uint8_t opcode2 = *(ip++);
      uint8_t mod = (opcode2 >> 6) & 3;
      uint8_t reg = (opcode2 >> 3) & 7;
      uint8_t rm  = (opcode2 >> 0) & 7;
      uint32_t displacement;
      switch(mod) {
        case 0:
          if(rm == 0b0110) {
            //immediate (32-bit) displacement is address
            displacement = readimm32(ip);
            ip += 4;
          }
          else {
            //no displacement
            displacement = 0;
          }
          break;
          
        case 1:
          //8-bit displacement
          displacement = readimm8(ip);
          ip += 1;
          break;
          
        case 2:
          //32-bit displacement
          displacement = readimm32(ip);
          ip += 4;
          break;
          
        case 3:
          break;
          
      }
    }
    break;
  }
}

void x86proc_modrm(x86state* ps, x86modrm* pmodrm)
{
  const uint8_t opc = *(ps->ip++);
  //split into mod, reg, and rm fields
  uint8_t mod = (opc >> 6) & 0b011;
  uint8_t reg = (opc >> 3) & 0b0111;
  uint8_t rm  = (opc >> 0) & 0b0111;
  //parse reg field (first operand)
  pmodrm->opd1 = (x86register)reg;
  //look at mod field
  switch(mod) {
    case 0b00:
      if(rm == 0b110) {
        //displacement is address
        pmodrm->opd2_reg = REG_NONE;
        pmodrm->opd2_displacement = (int32_t)readimm32(ps->ip); ps->ip += 4;
        pmodrm->opd2_address = ADDR_DISP;
      }
      else {
        //no displacement
        pmodrm->opd2_reg = REG_NONE;
        pmodrm->opd2_displacement = 0;
        pmodrm->opd2_address = (x86address)rm;
      }
      break;
    case 0b01:
      //8-bit displacement
      pmodrm->opd2_reg = REG_NONE;
      pmodrm->opd2_displacement = (int32_t)((int8_t)readimm8(ps->ip)); ps->ip += 1;
      pmodrm->opd2_address = (x86address)rm;
      break;
      
    case 0b10:
      //32-bit displacement
      pmodrm->opd2_reg = REG_NONE;
      pmodrm->opd2_displacement = (int32_t)readimm32(ps->ip); ps->ip += 4;
      pmodrm->opd2_address = (x86address)rm;
      break;
      
    case 0b11:
      //register operand
      pmodrm->opd2_reg = (x86register)rm;
      pmodrm->opd2_displacement = 0; //shouldn't be used
      pmodrm->opd2_address = ADDR_NONE;
      break;
  }
  //modrm field fully decoded
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
