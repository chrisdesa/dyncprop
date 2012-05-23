#include <stdlib.h>
#include <stdio.h>

#include "x86emu.h"

// on function call, return value is in EAX

int opcproc_unsupported(x86state* ps, x86opcodefamily* popcf, uint8_t opc);

int opcproc_alu(x86state* ps, x86opcodefamily* popcf, uint8_t opc);
int32_t opcprocalu_add(int32_t a, int32_t b, x86state* ps);
int32_t opcprocalu_or(int32_t a, int32_t b, x86state* ps);
int32_t opcprocalu_adc(int32_t a, int32_t b, x86state* ps);
int32_t opcprocalu_sbb(int32_t a, int32_t b, x86state* ps);
int32_t opcprocalu_and(int32_t a, int32_t b, x86state* ps);
int32_t opcprocalu_sub(int32_t a, int32_t b, x86state* ps);
int32_t opcprocalu_xor(int32_t a, int32_t b, x86state* ps);
int32_t opcprocalu_cmp(int32_t a, int32_t b, x86state* ps);

int opcproc_inc(x86state* ps, x86opcodefamily* popcf, uint8_t opc);
int opcproc_dec(x86state* ps, x86opcodefamily* popcf, uint8_t opc);
int opcproc_push(x86state* ps, x86opcodefamily* popcf, uint8_t opc);
int opcproc_pop(x86state* ps, x86opcodefamily* popcf, uint8_t opc);
int opcproc_xchgAX(x86state* ps, x86opcodefamily* popcf, uint8_t opc);

int opcproc_jrc8(x86state* ps, x86opcodefamily* popcf, uint8_t opc);

int opcproc_grp1(x86state* ps, x86opcodefamily* popcf, uint8_t opc);
int opcproc_grp2(x86state* ps, x86opcodefamily* popcf, uint8_t opc);

int opcproc_xchg(x86state* ps, x86opcodefamily* popcf, uint8_t opc);

int opcproc_pop_general(x86state* ps, x86opcodefamily* popcf, uint8_t opc);

int opcproc_mov(x86state* ps, x86opcodefamily* popcf, uint8_t opc);

int opcproc_call(x86state* ps, x86opcodefamily* popcf, uint8_t opc);

int opcproc_pushf(x86state* ps, x86opcodefamily* popcf, uint8_t opc);
int opcproc_popf(x86state* ps, x86opcodefamily* popcf, uint8_t opc);
int opcproc_sahf(x86state* ps, x86opcodefamily* popcf, uint8_t opc);
int opcproc_lahf(x86state* ps, x86opcodefamily* popcf, uint8_t opc);

int opcproc_mov_general_imm8(x86state* ps, x86opcodefamily* popcf, uint8_t opc);
int opcproc_mov_general_imm32(x86state* ps, x86opcodefamily* popcf, uint8_t opc);

int opcproc_ret_imm32(x86state* ps, x86opcodefamily* popcf, uint8_t opc);
int opcproc_ret(x86state* ps, x86opcodefamily* popcf, uint8_t opc);

int opcproc_jmp(x86state* ps, x86opcodefamily* popcf, uint8_t opc);

int opcproc_mov_imm8(x86state* ps, x86opcodefamily* popcf, uint8_t opc);
int opcproc_mov_imm32(x86state* ps, x86opcodefamily* popcf, uint8_t opc);

//complement/clear/set carry flag
int opcproc_cmc(x86state* ps, x86opcodefamily* popcf, uint8_t opc);
int opcproc_clc(x86state* ps, x86opcodefamily* popcf, uint8_t opc);
int opcproc_stc(x86state* ps, x86opcodefamily* popcf, uint8_t opc);

#define OPCF_UNSUPPORTED(opstr,msg) {opcproc_unsupported, { .unsupported = {opstr,msg}}}

#define OPCF_ALU(op,c,m) {opcproc_alu, { .alu = {opcprocalu_ ## op, c, m, 0, 0, 0}}}, \
                         {opcproc_alu, { .alu = {opcprocalu_ ## op, c, m, 1, 0, 0}}}, \
                         {opcproc_alu, { .alu = {opcprocalu_ ## op, c, m, 0, 1, 0}}}, \
                         {opcproc_alu, { .alu = {opcprocalu_ ## op, c, m, 1, 1, 0}}}, \
                         {opcproc_alu, { .alu = {opcprocalu_ ## op, c, m, 0, 0, 1}}}, \
                         {opcproc_alu, { .alu = {opcprocalu_ ## op, c, m, 1, 0, 1}}}
                         
#define OPCF_ALU1(op,c,m,w,d,imm) {opcproc_alu, { .alu = {opcprocalu_ ## op, c, m, w, d, imm}}}
                       
#define OPCF_B1(op) {opcproc_ ## op, { .b1 = { REG_EAX }}}, \
                    {opcproc_ ## op, { .b1 = { REG_ECX }}}, \
                    {opcproc_ ## op, { .b1 = { REG_EDX }}}, \
                    {opcproc_ ## op, { .b1 = { REG_EBX }}}, \
                    {opcproc_ ## op, { .b1 = { REG_ESP }}}, \
                    {opcproc_ ## op, { .b1 = { REG_EBP }}}, \
                    {opcproc_ ## op, { .b1 = { REG_ESI }}}, \
                    {opcproc_ ## op, { .b1 = { REG_EDI }}}
                    
#define OPCF_JRC8(cond) {opcproc_jrc8, { .jrc8 = { cond }}}

#define OPCF_GRP1(w,wi) {opcproc_grp1, { .grp1 = { w, wi }}}
#define OPCF_GRP2(w,cl) {opcproc_grp2, { .grp2 = { w, cl }}}

#define OPCF_XCHG(w) {opcproc_grp1, { .xchg = { w }}}

#define OPCF_MOV(w,d) {opcproc_mov, { .mov = { w, d }}}

#define OPCF_NODATA(op) {opcproc_ ## op, { .nodata = {}}}

#define OPCF_JMP(w) {opcproc_jmp, { .jmp = { w }}}

x86opcodefamily opc_families[256] = {
  OPCF_ALU(add,0,1),
  OPCF_UNSUPPORTED("PUSH ES","segment register push unsupported"),
  OPCF_UNSUPPORTED("POP ES","segment register pop unsupported"),
  
  OPCF_ALU(or,0,1),
  OPCF_UNSUPPORTED("PUSH CS","segment register push unsupported"),
  OPCF_UNSUPPORTED("[0F]","two-byte instructions unsupported"),
  
  OPCF_ALU(adc,1,1),
  OPCF_UNSUPPORTED("PUSH SS","segment register push unsupported"),
  OPCF_UNSUPPORTED("POP SS","segment register pop unsupported"),
  
  OPCF_ALU(sbb,1,1),
  OPCF_UNSUPPORTED("PUSH DS","segment register push unsupported"),
  OPCF_UNSUPPORTED("POP DS","segment register pop unsupported"),
  
  OPCF_ALU(and,0,1),
  OPCF_UNSUPPORTED("[ES:]","segment override prefix unsupported"),
  OPCF_UNSUPPORTED("DAA AL","decimal adjust instructions unsupported"),
  
  OPCF_ALU(sub,0,1),
  OPCF_UNSUPPORTED("[CS:]","segment override prefix unsupported"),
  OPCF_UNSUPPORTED("DAS AL","decimal adjust instructions unsupported"),
  
  OPCF_ALU(xor,0,1),
  OPCF_UNSUPPORTED("[SS:]","segment override prefix unsupported"),
  OPCF_UNSUPPORTED("AAA AL","ASCII adjust instructions unsupported"),
  
  OPCF_ALU(sub,0,0), //compare
  OPCF_UNSUPPORTED("[DS:]","segment override prefix unsupported"),
  OPCF_UNSUPPORTED("AAS AL","ASCII adjust instructions unsupported"),
  
  OPCF_B1(inc),
  
  OPCF_B1(dec),
  
  OPCF_B1(push),
  
  OPCF_B1(pop),
  
  OPCF_UNSUPPORTED("[60]","instruction not in original i386 ISA"),
  OPCF_UNSUPPORTED("[61]","instruction not in original i386 ISA"),
  OPCF_UNSUPPORTED("[62]","instruction not in original i386 ISA"),
  OPCF_UNSUPPORTED("[63]","instruction not in original i386 ISA"),
  OPCF_UNSUPPORTED("[64]","instruction not in original i386 ISA"),
  OPCF_UNSUPPORTED("[65]","instruction not in original i386 ISA"),
  OPCF_UNSUPPORTED("[66]","instruction not in original i386 ISA"),
  OPCF_UNSUPPORTED("[67]","instruction not in original i386 ISA"),
  
  OPCF_UNSUPPORTED("[68]","instruction not in original i386 ISA"),
  OPCF_UNSUPPORTED("[69]","instruction not in original i386 ISA"),
  OPCF_UNSUPPORTED("[6A]","instruction not in original i386 ISA"),
  OPCF_UNSUPPORTED("[6B]","instruction not in original i386 ISA"),
  OPCF_UNSUPPORTED("[6C]","instruction not in original i386 ISA"),
  OPCF_UNSUPPORTED("[6D]","instruction not in original i386 ISA"),
  OPCF_UNSUPPORTED("[6E]","instruction not in original i386 ISA"),
  OPCF_UNSUPPORTED("[6F]","instruction not in original i386 ISA"),
  
  OPCF_JRC8("o"),
  OPCF_JRC8("!o"),
  OPCF_JRC8("c"),
  OPCF_JRC8("!c"),
  OPCF_JRC8("z"),
  OPCF_JRC8("!z"),
  OPCF_JRC8("&cz"),
  OPCF_JRC8("&!c!z"),
  
  OPCF_JRC8("s"),
  OPCF_JRC8("!s"),
  OPCF_JRC8("p"),
  OPCF_JRC8("!p"),
  OPCF_JRC8("!&!&!so!&!os"),
  OPCF_JRC8("&!&!so!&!os"),
  OPCF_JRC8("!&!z!&!&!so!&!os"),
  OPCF_JRC8("&!z&!&!so!&!os"),
  
  OPCF_GRP1(0,0),
  OPCF_GRP1(1,1),
  OPCF_GRP1(0,0),
  OPCF_GRP1(1,0),
  OPCF_ALU1(and,0,0,0,0,0), //test
  OPCF_ALU1(and,0,0,1,0,0), //test
  OPCF_XCHG(0),
  OPCF_XCHG(1),
  
  OPCF_MOV(0,0),
  OPCF_MOV(1,0),
  OPCF_MOV(0,1),
  OPCF_MOV(1,1),
  OPCF_UNSUPPORTED("MOV Ew Sw","segmenting registers not supported"),
  OPCF_UNSUPPORTED("MOV Gv M","segmenting registers not supported"),
  OPCF_UNSUPPORTED("MOV Sw Ew","segmenting registers not supported"),
  OPCF_NODATA(pop_general),
  
  OPCF_B1(xchgAX), //also includes opcode 0x90, NOP, which is also XCHG AX,AX
  
  OPCF_UNSUPPORTED("CWDE","data conversion operations not supported"),
  OPCF_UNSUPPORTED("CDQ","data conversion operations not supported"),
  OPCF_UNSUPPORTED("CALL Ap","segmented call not supported"),
  OPCF_UNSUPPORTED("WAIT","wait operation not supported"),
  OPCF_NODATA(pushf),
  OPCF_NODATA(popf),
  OPCF_NODATA(sahf),
  OPCF_NODATA(lahf),
  
  OPCF_UNSUPPORTED("MOV AL Ob","unsure about semantics of these move ops"),
  OPCF_UNSUPPORTED("MOV AX Ov","unsure about semantics of these move ops"),
  OPCF_UNSUPPORTED("MOV Ob AL","unsure about semantics of these move ops"),
  OPCF_UNSUPPORTED("MOV Ov AX","unsure about semantics of these move ops"),
  OPCF_UNSUPPORTED("MOVSB","we probably don't need to deal with these string ops"),
  OPCF_UNSUPPORTED("MOVSW","we probably don't need to deal with these string ops"),
  OPCF_UNSUPPORTED("CMPSB","we probably don't need to deal with these string ops"),
  OPCF_UNSUPPORTED("CMPSW","we probably don't need to deal with these string ops"),
  
  OPCF_ALU1(and,0,0,0,0,1), //test imm
  OPCF_ALU1(and,0,0,1,0,0), //test imm
  OPCF_UNSUPPORTED("STOSB","we probably don't need to deal with these string ops"),
  OPCF_UNSUPPORTED("STOSW","we probably don't need to deal with these string ops"),
  OPCF_UNSUPPORTED("LODSB","we probably don't need to deal with these string ops"),
  OPCF_UNSUPPORTED("LODSW","we probably don't need to deal with these string ops"),
  OPCF_UNSUPPORTED("SCASB","we probably don't need to deal with these string ops"),
  OPCF_UNSUPPORTED("SCASW","we probably don't need to deal with these string ops"),
  
  OPCF_B1(mov_imm8),
  
  OPCF_B1(mov_imm32),
  
  OPCF_UNSUPPORTED("[C0]","instruction not in original i386 ISA"),
  OPCF_UNSUPPORTED("[C1]","instruction not in original i386 ISA"),
  OPCF_NODATA(ret_imm32),
  OPCF_NODATA(ret),
  OPCF_UNSUPPORTED("LES Gv Mp","segmenting registers not supported"),
  OPCF_UNSUPPORTED("LDS Gv Mp","segmenting registers not supported"),
  OPCF_NODATA(mov_general_imm8),
  OPCF_NODATA(mov_general_imm32),
  
  OPCF_UNSUPPORTED("[C8]","instruction not in original i386 ISA"),
  OPCF_UNSUPPORTED("[C9]","instruction not in original i386 ISA"),
  OPCF_UNSUPPORTED("RETF Iw","interrupt instructions not supported"),
  OPCF_UNSUPPORTED("RETF","interrupt instructions not supported"),
  OPCF_UNSUPPORTED("INT 3","interrupt instructions not supported"),
  OPCF_UNSUPPORTED("INT Ib","interrupt instructions not supported"),
  OPCF_UNSUPPORTED("RETF Iw","interrupt instructions not supported"),
  OPCF_UNSUPPORTED("RETF Iw","interrupt instructions not supported"),
  
  OPCF_GRP2(0,0),
  OPCF_GRP2(1,0),
  OPCF_GRP2(0,1),
  OPCF_GRP2(1,1),
  OPCF_UNSUPPORTED("AAM","we probably don't need to deal with these string ops"),
  OPCF_UNSUPPORTED("AAD","we probably don't need to deal with these string ops"),
  OPCF_UNSUPPORTED("[D6]","instruction not in original i386 ISA"),
  OPCF_UNSUPPORTED("XLAT","lookup table op not supported"),
  
  OPCF_UNSUPPORTED("[D8]","instruction not in original i386 ISA"),
  OPCF_UNSUPPORTED("[D9]","instruction not in original i386 ISA"),
  OPCF_UNSUPPORTED("[DA]","instruction not in original i386 ISA"),
  OPCF_UNSUPPORTED("[DB]","instruction not in original i386 ISA"),
  OPCF_UNSUPPORTED("[DC]","instruction not in original i386 ISA"),
  OPCF_UNSUPPORTED("[DD]","instruction not in original i386 ISA"),
  OPCF_UNSUPPORTED("[DE]","instruction not in original i386 ISA"),
  OPCF_UNSUPPORTED("[DF]","instruction not in original i386 ISA"),
  
  OPCF_UNSUPPORTED("LOOPNZ","GCC probably won't generate these jump ops"),
  OPCF_UNSUPPORTED("LOOPZ","GCC probably won't generate these jump ops"),
  OPCF_UNSUPPORTED("LOOP","GCC probably won't generate these jump ops"),
  OPCF_UNSUPPORTED("JCXZ","GCC probably won't generate these jump ops"),
  OPCF_UNSUPPORTED("IN AL Ib","port ops not supported"),
  OPCF_UNSUPPORTED("IN AX Ib","port ops not supported"),
  OPCF_UNSUPPORTED("IN Ib AL","port ops not supported"),
  OPCF_UNSUPPORTED("IN Ib AX","port ops not supported"),
  
  OPCF_NODATA(call),
  OPCF_JMP(1),
  OPCF_UNSUPPORTED("IN Ib AX","port ops not supported"),
  OPCF_JMP(0),
  OPCF_UNSUPPORTED("IN AL DX","port ops not supported"),
  OPCF_UNSUPPORTED("IN AX DX","port ops not supported"),
  OPCF_UNSUPPORTED("IN DX AL","port ops not supported"),
  OPCF_UNSUPPORTED("IN DX AX","port ops not supported"),
  
  OPCF_UNSUPPORTED("LOCK","lock not supported"),
  OPCF_UNSUPPORTED("[F1]","instruction not in original i386 ISA"),
  OPCF_UNSUPPORTED("REPNZ","gcc probably won't generate this"),
  OPCF_UNSUPPORTED("REPZ","gcc probably won't generate this"),
  OPCF_UNSUPPORTED("HALT","halt not supported"),
  OPCF_NODATA(cmc),
  OPCF_UNSUPPORTED("GRP3a","group3a instructions not supported"),
  OPCF_UNSUPPORTED("GRP3b","group3b instructions not supported"),
  
  OPCF_NODATA(clc),
  OPCF_NODATA(stc),
  OPCF_UNSUPPORTED("CLI","instruction not supported"),
  OPCF_UNSUPPORTED("STI","instruction not supported"),
  OPCF_UNSUPPORTED("CLD","instruction not supported"),
  OPCF_UNSUPPORTED("STD","instruction not supported"),
  OPCF_UNSUPPORTED("GRP4","group4 instructions not supported"),
  OPCF_UNSUPPORTED("GRP5","group5 instructions not supported")
  
};

#undef OPCF_UNSUPPORTED
#undef OPCF_ALU
#undef OPCF_B1
#undef OPCF_JRC8

int opcproc_unsupported(x86state* ps, x86opcodefamily* popcf, uint8_t opc)
{
  fprintf(stderr, "Error on instruction [%02X] => %s\n", opc, popcf->data.unsupported.op_str);
  fprintf(stderr, "  %s\n", popcf->data.unsupported.msg);
  exit(1);
}

int opcproc_alu(x86state* ps, x86opcodefamily* popcf, uint8_t opc)
{
  //make sure it's a 4-byte operation
  if(popcf->data.alu.W != 1) {
    fprintf(stderr, "Error: Byte alu instructions not supported (%s:%d).\n",__FILE__,__LINE__);
    exit(1);
  }
  //in the case of an immediate operation
  if(popcf->data.alu.IMM != 0) {
    fprintf(stderr, "Error: Immediate alu instructions not supported (%s:%d).\n",__FILE__,__LINE__);
    exit(1);
  }
  //we first need to parse the ModR/M byte
  const uint8_t* pinstr = ps->ip - 1;
  x86modrm modrm;
  x86proc_modrm(ps, &modrm);
  //check for kind of first operand
  if(modrm.opd1_reg != REG_NONE) {
    x86register reg_src, reg_dst;
    if(popcf->data.mov.D == 0) {
      reg_dst = modrm.opd1_reg;
      reg_src = modrm.opd2;
    }
    else {
      reg_dst = modrm.opd2;
      reg_src = modrm.opd1_reg;
    }
    x86datastate rs_src, rs_dst, fs_c;
    rs_src = ps->regs[reg_src].state;
    rs_dst = ps->regs[reg_dst].state;
    if(popcf->data.alu.C == 0) {
      //if the op doesn't depend on the carry flag, act like it's real
      fs_c = DS_REAL;
    }
    else {
      //otherwise, we get the state of the carry flag
      fs_c = ps->flags[FLAG_C].state;
    }
    if((rs_src == DS_REAL)&&(rs_dst == DS_REAL)&&(fs_c == DS_REAL)) {
      //all operands are real; we don't need to emit anything
      int32_t r = popcf->data.alu.procalufx(ps->regs[reg_dst].value, ps->regs[reg_src].value, ps);
      ps->regs[reg_dst].state = DS_REAL;
      ps->regs[reg_dst].value = r;
    }
    else {
      //we need to emit
      if(rs_src == DS_REAL) {
        //first setup the source register
        uint8_t ee[5];
        uint32_t v = ps->regs[reg_src].value;
        ee[0] = 0xB8 + reg_src;
        ee[1] = ((v >> 0) & 0xFF);
        ee[2] = ((v >> 8) & 0xFF);
        ee[3] = ((v >> 16) & 0xFF);
        ee[4] = ((v >> 24) & 0xFF);
        x86emit(ps, ee, 5);
      }
      if((rs_dst == DS_REAL)&&(reg_src != reg_dst)) {
        //we now need to set up the destination register
        uint8_t ee[5];
        uint32_t v = ps->regs[reg_dst].value;
        ee[0] = 0xB8 + reg_dst;
        ee[1] = ((v >> 0) & 0xFF);
        ee[2] = ((v >> 8) & 0xFF);
        ee[3] = ((v >> 16) & 0xFF);
        ee[4] = ((v >> 24) & 0xFF);
        x86emit(ps, ee, 5);
      }
      if((fs_c == DS_REAL)&&(popcf->data.alu.C)) {
        //we need to initialize the carry flag
        if(ps->flags[FLAG_C].value) {
          x86emit1(ps, 0xF9); //set carry flag
        }
        else {
          x86emit1(ps, 0xF8); //clear carry flag
        }
      }
      //now emit the original instruction
      x86emit(ps, pinstr, ps->ip - pinstr);
      //finally, do the assignment in our model
      if(((rs_dst == DS_STACK_PTR)&&(rs_src == DS_REAL))||((rs_dst == DS_REAL)&&(rs_src == DS_STACK_PTR))) {
        if((popcf->data.alu.procalufx == opcprocalu_add)||(popcf->data.alu.procalufx == opcprocalu_sub)) {
          //this still acts like a real op, even though we had to do an emit
          int32_t r = popcf->data.alu.procalufx(ps->regs[reg_dst].value, ps->regs[reg_src].value, ps);
          ps->regs[reg_dst].value = r;
          ps->regs[reg_dst].state = DS_STACK_PTR;
        }
        else {
          fprintf(stderr, "Error: Invalid alu operation on stack pointer (%s:%d).\n",__FILE__,__LINE__);
          exit(1);
        }
      }
      else {
        ps->regs[reg_dst].value = 0;
        ps->regs[reg_dst].state = DS_SYMBOLIC;
        //all alu ops write all flags
        ps->flags[FLAG_C] = x86data_init(DS_SYMBOLIC,0);
        ps->flags[FLAG_P] = x86data_init(DS_SYMBOLIC,0);
        ps->flags[FLAG_A] = x86data_init(DS_SYMBOLIC,0);
        ps->flags[FLAG_Z] = x86data_init(DS_SYMBOLIC,0);
        ps->flags[FLAG_S] = x86data_init(DS_SYMBOLIC,0);
        ps->flags[FLAG_O] = x86data_init(DS_SYMBOLIC,0);
      }
    }
  }
  else {
    fprintf(stderr, "Error: ALU ops in memory not supported (%s:%d).\n",__FILE__,__LINE__);
    exit(1);
  }
  //return
  return 0;
}

int32_t opcprocalu_add(int32_t a, int32_t b, x86state* ps)
{
  fprintf(stderr, "Unimplemented instruction (%s:%d)\n",__FILE__,__LINE__);
  exit(1);
}

int32_t opcprocalu_or(int32_t a, int32_t b, x86state* ps)
{
  fprintf(stderr, "Unimplemented instruction (%s:%d)\n",__FILE__,__LINE__);
  exit(1);
}

int32_t opcprocalu_adc(int32_t a, int32_t b, x86state* ps)
{
  fprintf(stderr, "Unimplemented instruction (%s:%d)\n",__FILE__,__LINE__);
  exit(1);
}

int32_t opcprocalu_sbb(int32_t a, int32_t b, x86state* ps)
{
  fprintf(stderr, "Unimplemented instruction (%s:%d)\n",__FILE__,__LINE__);
  exit(1);
}

int32_t opcprocalu_and(int32_t a, int32_t b, x86state* ps)
{
  fprintf(stderr, "Unimplemented instruction (%s:%d)\n",__FILE__,__LINE__);
  exit(1);
}

int32_t opcprocalu_sub(int32_t a, int32_t b, x86state* ps)
{
  fprintf(stderr, "Unimplemented instruction (%s:%d)\n",__FILE__,__LINE__);
  exit(1);
}

int32_t opcprocalu_xor(int32_t a, int32_t b, x86state* ps)
{
  fprintf(stderr, "Unimplemented instruction (%s:%d)\n",__FILE__,__LINE__);
  exit(1);
}

int32_t opcprocalu_cmp(int32_t a, int32_t b, x86state* ps)
{
  fprintf(stderr, "Unimplemented instruction (%s:%d)\n",__FILE__,__LINE__);
  exit(1);
}

int opcproc_inc(x86state* ps, x86opcodefamily* popcf, uint8_t opc)
{
  fprintf(stderr, "Unimplemented instruction (%s:%d)\n",__FILE__,__LINE__);
  exit(1);
}

int opcproc_dec(x86state* ps, x86opcodefamily* popcf, uint8_t opc)
{
  fprintf(stderr, "Unimplemented instruction (%s:%d)\n",__FILE__,__LINE__);
  exit(1);
}

int opcproc_push(x86state* ps, x86opcodefamily* popcf, uint8_t opc)
{
  //verify that the stack pointer actually does point to the stack
  if(ps->regs[REG_ESP].state != DS_STACK_PTR) {
    fprintf(stderr,"Error: On push, stack pointer did not point to stack.\n");
    exit(1);
  }
  //modify the stack pointer
  ps->regs[REG_ESP].value -= 4;
  //get the register that is being operated on
  x86register opd = popcf->data.b1.opd;
  //do the memory write
  x86stack_write32(ps, ps->regs[REG_ESP].value, ps->regs[opd]);
  //to keep the stack pointer where it is in the original code, we always
  //emit a push.  In this case, this is always the existing instruction.
  x86emit(ps, &opc, 1);
  //return
  return 0;
}

int opcproc_pop(x86state* ps, x86opcodefamily* popcf, uint8_t opc)
{
  //verify that the stack pointer actually does point to the stack
  if(ps->regs[REG_ESP].state != DS_STACK_PTR) {
    fprintf(stderr,"Error: On pop, stack pointer did not point to stack.\n");
    exit(1);
  }
  //get the register that is being operated on
  x86register opd = popcf->data.b1.opd;
  //do the memory read
  ps->regs[opd] = x86stack_read32(ps, ps->regs[REG_ESP].value);
  //modify the stack pointer
  ps->regs[REG_ESP].value += 4;
  //to keep the stack pointer where it is in the original code, we always
  //emit a pop.  In this case, this is always the existing instruction.
  x86emit(ps, &opc, 1);
  //return
  return 0;
}

int opcproc_jrc8(x86state* ps, x86opcodefamily* popcf, uint8_t opc)
{
  fprintf(stderr, "Unimplemented instruction (%s:%d)\n",__FILE__,__LINE__);
  exit(1);
}

int opcproc_grp1(x86state* ps, x86opcodefamily* popcf, uint8_t opc)
{
  fprintf(stderr, "Unimplemented instruction (%s:%d)\n",__FILE__,__LINE__);
  exit(1);
}

int opcproc_xchg(x86state* ps, x86opcodefamily* popcf, uint8_t opc)
{
  fprintf(stderr, "Unimplemented instruction (%s:%d)\n",__FILE__,__LINE__);
  exit(1);
}

int opcproc_mov(x86state* ps, x86opcodefamily* popcf, uint8_t opc)
{
  //make sure it's a 4-byte operation
  if(popcf->data.mov.W != 1) {
    fprintf(stderr, "Error: Byte mov instruction not supported (%s:%d).\n",__FILE__,__LINE__);
    exit(1);
  }
  //we first need to parse the ModR/M byte
  const uint8_t* pinstr = ps->ip - 1;
  x86modrm modrm;
  x86proc_modrm(ps, &modrm);
  //check for kind of first operand
  if(modrm.opd1_reg != REG_NONE) {
    x86register reg_src, reg_dst;
    if(popcf->data.mov.D == 0) {
      reg_dst = modrm.opd1_reg;
      reg_src = modrm.opd2;
    }
    else {
      reg_dst = modrm.opd2;
      reg_src = modrm.opd1_reg;
    }
    if(issymbolic(ps->regs[reg_src].state)) {
      //we have to emit the instruction
      uint8_t ee[2];
      ee[0] = opc;
      //remake the modrm byte
      ee[1] = (3 << 6) | (modrm.opd2 << 3) | (modrm.opd1_reg);
      x86emit(ps, ee, 2);
    }
    //assign the memory
    ps->regs[reg_dst] = ps->regs[reg_src];
  }
  else {
    if(popcf->data.mov.D == 0) {
      //this is a store
      fprintf(stderr, "Error: Stores not supported (%s:%d).\n",__FILE__,__LINE__);
      exit(1);
    }
    else {
      //this is a load; get the address
      x86data daddr = x86make_address(ps, modrm.opd1_address, modrm.opd1_displacement);
      switch(daddr.state) {
        case DS_REAL:
          fprintf(stderr, "Error: Real reads not supported (%s:%d).\n",__FILE__,__LINE__);
          exit(1);
          break;
        case DS_STACK_PTR:
          //do the read
          ps->regs[modrm.opd2] = x86stack_read32(ps, daddr.value);
          if(issymbolic(ps->regs[modrm.opd2].state)) {
            //we actually have to emit the load, since it was symbolic
            x86emit(ps, pinstr, ps->ip - pinstr);
          }
          break;
        case DS_SYMBOLIC:
          fprintf(stderr, "Error: Symbolic reads not supported (%s:%d).\n",__FILE__,__LINE__);
          exit(1);
          break;
        default:
          fprintf(stderr, "Error: Unrecognized address state (%s:%d).\n",__FILE__,__LINE__);
          exit(1);
      }
    }
  }
  //return
  return 0;
}

//pop a general operand (POP Ev)
int opcproc_pop_general(x86state* ps, x86opcodefamily* popcf, uint8_t opc)
{
  fprintf(stderr, "Unimplemented instruction (%s:%d)\n",__FILE__,__LINE__);
  exit(1);
}

int opcproc_call(x86state* ps, x86opcodefamily* popcf, uint8_t opc)
{
  fprintf(stderr, "Unimplemented instruction (%s:%d)\n",__FILE__,__LINE__);
  exit(1);
}

int opcproc_pushf(x86state* ps, x86opcodefamily* popcf, uint8_t opc)
{
  fprintf(stderr, "Unimplemented instruction (%s:%d)\n",__FILE__,__LINE__);
  exit(1);
}

int opcproc_popf(x86state* ps, x86opcodefamily* popcf, uint8_t opc)
{
  fprintf(stderr, "Unimplemented instruction (%s:%d)\n",__FILE__,__LINE__);
  exit(1);
}

int opcproc_sahf(x86state* ps, x86opcodefamily* popcf, uint8_t opc)
{
  fprintf(stderr, "Unimplemented instruction (%s:%d)\n",__FILE__,__LINE__);
  exit(1);
}

int opcproc_lahf(x86state* ps, x86opcodefamily* popcf, uint8_t opc)
{
  fprintf(stderr, "Unimplemented instruction (%s:%d)\n",__FILE__,__LINE__);
  exit(1);
}

int opcproc_xchgAX(x86state* ps, x86opcodefamily* popcf, uint8_t opc)
{
  fprintf(stderr, "Unimplemented instruction (%s:%d)\n",__FILE__,__LINE__);
  exit(1);
}

int opcproc_mov_imm8(x86state* ps, x86opcodefamily* popcf, uint8_t opc)
{
  fprintf(stderr, "Unimplemented instruction (%s:%d)\n",__FILE__,__LINE__);
  exit(1);
}

int opcproc_mov_imm32(x86state* ps, x86opcodefamily* popcf, uint8_t opc)
{
  fprintf(stderr, "Unimplemented instruction (%s:%d)\n",__FILE__,__LINE__);
  exit(1);
}

int opcproc_ret_imm32(x86state* ps, x86opcodefamily* popcf, uint8_t opc)
{
  fprintf(stderr, "Unimplemented instruction (%s:%d)\n",__FILE__,__LINE__);
  exit(1);
}

int opcproc_ret(x86state* ps, x86opcodefamily* popcf, uint8_t opc)
{
  //verify that the stack pointer actually does point to the stack
  if(ps->regs[REG_ESP].state != DS_STACK_PTR) {
    fprintf(stderr,"Error: On return, stack pointer did not point to stack (%s:%d)\n",__FILE__,__LINE__);
    exit(1);
  }
  //do the memory read
  x86data ret_addr = x86stack_read32(ps, ps->regs[REG_ESP].value);
  //verify that the return address has the correct state
  if(ret_addr.state != DS_RET_ADDR) {
    fprintf(stderr,"Error: On return, stack pointer did not point to return address, but instead had state [%s] (%s:%d)\n",
            x86datastate_tostr(ret_addr.state),__FILE__,__LINE__);
    exit(1);
  }
  //verify that BP holds the correct value
  if(ps->regs[REG_EBP].state != DS_RET_BP) {
    fprintf(stderr,"Error: On return, value of EPB wasn't restored (%s:%d)\n",__FILE__,__LINE__);
    exit(1);
  }
  //if the return value is real, we must set it
  if(ps->regs[REG_EAX].state == DS_REAL) {
    uint8_t ee[5];
    uint32_t v = ps->regs[REG_EAX].value;
    ee[0] = 0xB8 + REG_EAX;
    ee[1] = ((v >> 0) & 0xFF);
    ee[2] = ((v >> 8) & 0xFF);
    ee[3] = ((v >> 16) & 0xFF);
    ee[4] = ((v >> 24) & 0xFF);
    x86emit(ps, ee, 5);
  }
  //emit the return
  x86emit(ps, &opc, 1);
  //return from the function
  return 1;
}

int opcproc_grp2(x86state* ps, x86opcodefamily* popcf, uint8_t opc)
{
  fprintf(stderr, "Unimplemented instruction (%s:%d)\n",__FILE__,__LINE__);
  exit(1);
}

int opcproc_jmp(x86state* ps, x86opcodefamily* popcf, uint8_t opc)
{
  fprintf(stderr, "Unimplemented instruction (%s:%d)\n",__FILE__,__LINE__);
  exit(1);
}

int opcproc_mov_general_imm8(x86state* ps, x86opcodefamily* popcf, uint8_t opc)
{
  fprintf(stderr, "Unimplemented instruction (%s:%d)\n",__FILE__,__LINE__);
  exit(1);
}

int opcproc_mov_general_imm32(x86state* ps, x86opcodefamily* popcf, uint8_t opc)
{
  fprintf(stderr, "Unimplemented instruction (%s:%d)\n",__FILE__,__LINE__);
  exit(1);
}

int opcproc_cmc(x86state* ps, x86opcodefamily* popcf, uint8_t opc)
{
  fprintf(stderr, "Unimplemented instruction (%s:%d)\n",__FILE__,__LINE__);
  exit(1);
}

int opcproc_clc(x86state* ps, x86opcodefamily* popcf, uint8_t opc)
{
  fprintf(stderr, "Unimplemented instruction (%s:%d)\n",__FILE__,__LINE__);
  exit(1);
}

int opcproc_stc(x86state* ps, x86opcodefamily* popcf, uint8_t opc)
{
  fprintf(stderr, "Unimplemented instruction (%s:%d)\n",__FILE__,__LINE__);
  exit(1);
}
