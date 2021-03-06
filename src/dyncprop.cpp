#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

#include "dyncprop.h"
#include "Data.hpp"
#include "State.hpp"
#include "Home.hpp"
#include "Instr.hpp"

using namespace Dyncprop;

namespace Dyncprop {
  uint8_t* code_scratch = NULL;
}

void* dyncprop(void* pfn, const char* format, ...)
{
  //determine the number of arguments
  uint32_t nargs = strlen(format);
  
  //create an x86 State object to emulate over
  State s;
  
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
        s.stack_at(-4*(nargs-ia)) = data(DS_SYMBOLIC,0);
        break;
        
      case 'u':
      case 'd':
      case 'p':
        //real 4-byte argument
        s.stack_at(-4*(nargs-ia)) = data(DS_REAL,va_arg(ap, int));
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
  s.stack_at(-4*(nargs+1)) = data(DS_RET_ADDR,0);
  
  //initialize stack pointer to point to the last used entry
  s.regs[REG_ESP] = data(DS_STACK_PTR, -4*(nargs+1));
  
  //initialize callee-save registers, symbolically
  s.regs[REG_EBP] = data(DS_RET_BP, 0);
  s.regs[REG_EBX] = data(DS_RET_BX, 0);
  s.regs[REG_ESI] = data(DS_RET_SI, 0);
  s.regs[REG_EDI] = data(DS_RET_DI, 0);
  
  //set up the instruction pointer
  s.ip = (const uint8_t*)pfn;
  
  //we are now ready to begin stepping
  //need to have some mechanism here for dealing with symbolic conditional jumps,
  //which create two flows of execution
  while(s.step() == 0);
  
  //allocate some memory for the function to be stored at
  uint32_t pagesize = sysconf(_SC_PAGE_SIZE);
  uint32_t npages = (s.emitbuf.size() + pagesize - 1) / pagesize;
  void* rv = mmap(NULL, npages*pagesize, PROT_EXEC | PROT_WRITE | PROT_READ, MAP_PRIVATE | MAP_ANON, -1, 0);
  if(rv == MAP_FAILED) {
    fprintf(stderr,"Error:  Call to mmap failed.\n");
    exit(1);
  }
  
  //copy over the function
  memcpy(rv, &(s.emitbuf[0]), s.emitbuf.size());
  
  //disallow writing and reading the memory
  if(mprotect(rv, pagesize*npages, PROT_EXEC) != 0) {
    fprintf(stderr,"Error:  Call to mprotect failed.\n");
    exit(1);
  }
  
  //return the address of the function code
  return rv;
  
  //the destructor will free all the resources associated with the virtual machine state
}

void dyncprop_init()
{  
  //allocate some memory for the function to be stored at
  uint32_t pagesize = sysconf(_SC_PAGE_SIZE);
  void* rv = mmap(NULL, pagesize, PROT_EXEC | PROT_WRITE | PROT_READ, MAP_PRIVATE | MAP_ANON, -1, 0);
  if(rv == MAP_FAILED) {
    fprintf(stderr,"Error:  Call to mmap failed.\n");
    exit(1);
  }
  code_scratch = (uint8_t*)rv;
}


