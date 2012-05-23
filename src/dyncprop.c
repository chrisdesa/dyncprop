#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

#include "dyncprop.h"
#include "x86emu.h"

void dyncprop_initstate(x86state* ps);

void* dyncprop(void* pfn, const char* format, ...)
{
  //determine the number of arguments
  uint32_t nargs = strlen(format);
  
  //create an x86state object to emulate over
  x86state s;
  dyncprop_initstate(&s);
  
  //push the arguments onto the stack
  va_list ap;
  va_start(ap, format);
  for(int ia = 0; ia < nargs; ia++) {
    char fc = format[ia];
    switch(fc) {
      case 'U':
      case 'D':
      case 'P':
        //symbolic 4-byte argument
        x86stack_write32(&s, -4*(nargs-ia), x86data_init(DS_SYMBOLIC,0));
        break;
        
      case 'u':
      case 'd':
      case 'p':
        //real 4-byte argument
        x86stack_write32(&s, -4*(nargs-ia), x86data_init(DS_REAL,va_arg(ap, int)));
        break;
        
      case 'f':
      case 'F':
        fprintf(stderr,"Error: Floating point math not supported.\n");
        exit(1);
        
      default:
        fprintf(stderr,"Error: In dyncprop format string, unrecognized character.\n");
        exit(1);
    }
  }
  va_end(ap);
  
  //push return address onto the stack
  x86stack_write32(&s, -4*(nargs+1), x86data_init(DS_RET_ADDR,0));
  
  //initialize stack pointer to point to the last used entry
  s.regs[REG_ESP] = x86data_init(DS_STACK_PTR, -4*(nargs+1));
  
  //initialize base pointer, symbolically
  s.regs[REG_EBP] = x86data_init(DS_RET_BP, 0);
  
  //set up the instruction pointer
  s.ip = pfn;
  
  //we are now ready to begin stepping
  //need to have some mechanism here for dealing with symbolic conditional jumps,
  //which create two flows of execution
  while(x86step(&s) == 0);
  
  //assert that BP has been properly restored
  if(s.regs[REG_EBP].state != DS_RET_BP) {
    fprintf(stderr,"Error:  On function return, EBP is apparently not properly restored.\n");
    exit(1);
  }
  
  //free the stack
  free(s.pstack);
  
  //allocate some memory for the function to be stored at
  uint32_t pagesize = sysconf(_SC_PAGE_SIZE);
  uint32_t npages = (s.emitidx + pagesize - 1) / pagesize;
  void* rv = mmap(NULL, npages*pagesize, PROT_EXEC | PROT_WRITE | PROT_READ, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
  if(rv == MAP_FAILED) {
    fprintf(stderr,"Error:  Call to mmap failed.\n");
    exit(1);
  }
  
  //copy over the function and free the emit buffer
  memcpy(rv, s.emitbuf, s.emitidx);
  free(s.emitbuf);
  
  //disallow writing and reading the memory
  if(mprotect(rv, pagesize*npages, PROT_EXEC) != 0) {
    fprintf(stderr,"Error:  Call to mprotect failed.\n");
    exit(1);
  }
  
  //return the address of the function code
  return rv;
}

void dyncprop_initstate(x86state* ps)
{
  uint32_t i;
  for(i = 0; i < REG_ENUM_MAX; i++) {
    ps->regs[i] = x86data_init(DS_UNINITIALIZED, 0);
  }
  //flags
  for(i = 0; i < FLAG_ENUM_MAX; i++) {
    ps->flags[i] = x86data_init(DS_UNINITIALIZED, 0);
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

