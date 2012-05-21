#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

#include "dyncprop.h"
#include "x86emu.h"

void dyncprop_initstate(x86state* ps);

void* dyncprop(void* pfn, const char* format, ...)
{
  va_list ap;
  va_start(ap, format);
  
  x86state s;
  dyncprop_initstate(&s);
  s.regs[REG_ESP] = x86data_stack_ptr(0);
  s.regs[REG_EBP] = x86data_stack_ptr(0);
  s.ip = pfn;
  
  va_end(ap);
}

void dyncprop_initstate(x86state* ps)
{
  uint32_t i;
  for(i = 0; i < REG_ENUM_MAX; i++) {
    ps->regs[i] = x86data_uninitialized();
  }
  //flags
  for(i = 0; i < FLAG_ENUM_MAX; i++) {
    ps->flags[i] = x86data_uninitialized();
  }
  //instruction pointer
  ps->ip = NULL;
  //stack
  ps->stacksz = 1024;
  ps->pstack = malloc(sizeof(x86data)*ps->stacksz);
  //emission pointer and emission buffer size
  ps->emitbuf = malloc(1024);
  ps->emitidx = 0;
  ps->emitsz = 1024;
}

