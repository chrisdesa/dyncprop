
#include <stdint.h>

typedef enum x86register {
  //general purpose registers
  REG_EAX = 0,
  REG_ECX = 1,
  REG_EDX = 2,
  REG_EBX = 3,
  //pointer registers
  REG_ESP = 4,
  REG_EBP = 5,
  //index registers
  REG_ESI = 6,
  REG_EDI = 7,
  //maximum value of enum
  REG_ENUM_MAX = 8,
  //no register
  REG_NONE = -1
} x86register;

typedef enum x86address {
  //r/m operand modes
  ADDR_BX_SI_DISP = 0,
  ADDR_BX_DI_DISP = 1,
  ADDR_BP_SI_DISP = 2,
  ADDR_BP_DI_DISP = 3,
  ADDR_SI_DISP = 4,
  ADDR_DI_DISP = 5,
  ADDR_BP_DISP = 6,
  ADDR_BX_DISP = 7,
  //extra mode for when mod = 00, r/m = 110 (absolute addressing)
  ADDR_DISP = 8,
  //maximum value of enum
  ADDR_ENUM_MAX = 9,
  //no address
  ADDR_NONE = -1
} x86address;

typedef struct x86data {
  int32_t value;
  enum {
    uninitialized = 0,
    real = 1,
    symbolic = 2
  } state;
} x86data;

//all the supported x86 flags
typedef struct x86flags {
  x86register c; //carry
  x86register p; //parity
  x86register a; //adjust
  x86register z; //zero
  x86register s; //sign
  x86register o; //overflow
} x86flags;

typedef struct x86state {
  //general purpose registers
  x86data regs[8];
  //flags
  x86flags flags;
  //instruction pointer
  const uint8_t* ip;
} x86state;


typedef struct x86modrm {
  //operand 1 (either a register, or a memory access with a register and an offset)
  x86register opd1_reg; //REG_NONE if memory access
  int32_t opd1_displacement;  //the displacement value
  x86address opd1_address; //ADDR_NONE if register value
  //operand 2 (register)
  x86register opd2;
} x86modrm;

typedef struct x86opcodefamily x86opcodefamily;

typedef void (*x86opcodeproc)(x86state* ps, x86opcodefamily* popcf, uint8_t opc);
typedef int32_t (*x86opcodeproc_alu)(int32_t a, int32_t b, x86flags* pflags);

struct x86opcodefamily {
  //processing function
  x86opcodeproc procfx;
  //data for the instruction function
  union {
    //when no data is needed
    struct {
      
    } nodata;
    //data for unsupported instructions
    struct {
      const char* op_str;
      const char* msg;
    } unsupported;
    //data for ALU family instructions
    //ALU family instructions have 2 operands, and come in groups of 6
    //they do not touch memory
    struct {
      //function to run to perform the ALU op
      x86opcodeproc_alu procalufx;
      //does the OP depend of the carry flag?
      uint32_t C;
      //does the OP modify its result operand?
      uint32_t M;
      //width bit (0 = 8-bit, 1 = 32-bit)
      uint32_t W;
      //direction bit (0 = op1 is source, 1 = op1 is destination)
      uint32_t D;
      //immediate mode bit (overrides D, operates only on EAX/AL)
      uint32_t IMM;
    } alu;
    //data for B1 group instructions
    //B1 group instructions have 1 operand, which is encoded in the single-byte opcode
    //includes INC, DEC, PUSH, and POP
    struct {
      //function to run to perform the B1 op
      x86register opd;
    } b1;
    //data for jrc8 instructions
    //these are Jump Relative Condition 8-bit instructions
    struct {
      //cond is a string that describes the condition in terms of flags
      const char* cond;
    } jrc8;
    //data for GRP1 instructions
    struct {
      //width of the operand (0 = 8-bit, 1 = 32-bit)
      uint32_t W;
      //width of the immediate (0 = 8-bit, 1 = 32-bit)
      uint32_t WI;
    } grp1;
    //data for general-register xchg operation
    //xchg with EAX is a B1-group instruction
    struct {
      //width of the operand (0 = 8-bit, 1 = 32-bit)
      uint32_t W;
    } xchg;
    //data for general-register mov operation
    //MOV immediate is a B1-group instruction
    struct {
      //width bit (0 = 8-bit, 1 = 32-bit)
      uint32_t W;
      //direction bit (0 = op1 is source, 1 = op1 is destination)
      uint32_t D;
    } mov;
    //data for GRP2 instructions
    struct {
      //width of the operand (0 = 8-bit, 1 = 32-bit)
      uint32_t W;
      //second operand (0 = constant 1, 1 = CL)
      uint32_t CL;
    } grp2;
    //data for jmp instruction
    struct {
      //width of the operand (0 = 8-bit, 1 = 32-bit)
      uint32_t W;
    } jmp;
  } data;
};

extern x86opcodefamily opc_families[256];

void x86step(x86state* ps);

void x86proc_modrm(x86state* ps, x86modrm* pmodrm);

