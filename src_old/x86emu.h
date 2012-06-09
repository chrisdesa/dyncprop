
#include <stdint.h>

//emission maximum is 16kB of code
#define EMIT_MAX (16*1024)

//stack max is 64KB of stack
#define STACK_MAX (16*1024)

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

typedef enum x86flag {
  FLAG_C = 0,
  FLAG_P = 2,
  FLAG_A = 4,
  FLAG_Z = 6,
  FLAG_S = 7,
  FLAG_O = 11,
  FLAG_ENUM_MAX = 16
} x86flag;

typedef enum x86datastate {
  DS_UNINITIALIZED = 0,
  DS_REAL = 1,
  DS_SYMBOLIC = 2,
  DS_STACK_PTR = 3,
  DS_RET_ADDR = 4,  //return address
  DS_RET_BP = 5, //initial value of BP
} x86datastate;

typedef struct x86data {
  x86datastate state;
  int32_t value;
} x86data;

x86data x86data_init(x86datastate state, int32_t value);

typedef struct x86state {
  //general purpose registers
  x86data regs[8];
  //flags
  x86data flags[16];
  //instruction pointer
  const uint8_t* ip;
  //stack
  x86data* pstack;
  uint32_t stacksz;
  //emission pointer and emission buffer size
  uint8_t* emitbuf;
  uint32_t emitidx;
  uint32_t emitsz;
} x86state;


typedef struct x86modrm {
  //operand 1 (either a register, or a memory access with a register and an offset)
  x86register opd1_reg; //REG_NONE if memory access
  int32_t opd1_displacement;  //the displacement value
  x86register opd1_address; //ADDR_NONE if only displacement
  //operand 2 (register)
  x86register opd2;
} x86modrm;

typedef struct x86opcodefamily x86opcodefamily;

typedef int (*x86opcodeproc)(x86state* ps, x86opcodefamily* popcf, uint8_t opc);
typedef int32_t (*x86opcodeproc_alu)(int32_t a, int32_t b, x86state* ps);

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

uint8_t readimm8(const uint8_t* ip);
uint16_t readimm16(const uint8_t* ip);
uint32_t readimm32(const uint8_t* ip);

int x86step(x86state* ps);

void x86proc_modrm(x86state* ps, x86modrm* pmodrm);

void x86emit(x86state* ps, const uint8_t* instr, uint32_t len);
void x86emit1(x86state* ps, uint8_t instr);

void x86stack_write32(x86state* ps, int32_t addr, x86data value);
x86data x86stack_read32(x86state* ps, int32_t addr);
x86data* x86stack_access32(x86state* ps, int32_t addr);

int issymbolic(x86datastate ds);
int isreal(x86datastate ds);

const char* x86datastate_tostr(x86datastate ds);
const char* x86register_tostr(x86register reg);

x86data x86make_address(x86state* ps, x86register addr, int32_t offset);
