#include <stdlib.h>
#include <stdio.h>

#include "x86emu.h"

// on function call, return value is in EAX

void opcproc_unsupported(x86state* ps, x86opcodefamily* popcf, uint8_t opc);

void opcproc_alu(x86state* ps, x86opcodefamily* popcf, uint8_t opc);
int32_t opcprocalu_add(int32_t a, int32_t b, x86state* ps);
int32_t opcprocalu_or(int32_t a, int32_t b, x86state* ps);
int32_t opcprocalu_adc(int32_t a, int32_t b, x86state* ps);
int32_t opcprocalu_sbb(int32_t a, int32_t b, x86state* ps);
int32_t opcprocalu_and(int32_t a, int32_t b, x86state* ps);
int32_t opcprocalu_sub(int32_t a, int32_t b, x86state* ps);
int32_t opcprocalu_xor(int32_t a, int32_t b, x86state* ps);
int32_t opcprocalu_cmp(int32_t a, int32_t b, x86state* ps);

void opcproc_inc(x86state* ps, x86opcodefamily* popcf, uint8_t opc);
void opcproc_dec(x86state* ps, x86opcodefamily* popcf, uint8_t opc);
void opcproc_push(x86state* ps, x86opcodefamily* popcf, uint8_t opc);
void opcproc_pop(x86state* ps, x86opcodefamily* popcf, uint8_t opc);
void opcproc_xchgAX(x86state* ps, x86opcodefamily* popcf, uint8_t opc);

void opcproc_jrc8(x86state* ps, x86opcodefamily* popcf, uint8_t opc);

void opcproc_grp1(x86state* ps, x86opcodefamily* popcf, uint8_t opc);
void opcproc_grp2(x86state* ps, x86opcodefamily* popcf, uint8_t opc);

void opcproc_xchg(x86state* ps, x86opcodefamily* popcf, uint8_t opc);

void opcproc_pop_general(x86state* ps, x86opcodefamily* popcf, uint8_t opc);

void opcproc_mov(x86state* ps, x86opcodefamily* popcf, uint8_t opc);

void opcproc_call(x86state* ps, x86opcodefamily* popcf, uint8_t opc);

void opcproc_pushf(x86state* ps, x86opcodefamily* popcf, uint8_t opc);
void opcproc_popf(x86state* ps, x86opcodefamily* popcf, uint8_t opc);
void opcproc_sahf(x86state* ps, x86opcodefamily* popcf, uint8_t opc);
void opcproc_lahf(x86state* ps, x86opcodefamily* popcf, uint8_t opc);

void opcproc_mov_general_imm8(x86state* ps, x86opcodefamily* popcf, uint8_t opc);
void opcproc_mov_general_imm32(x86state* ps, x86opcodefamily* popcf, uint8_t opc);

void opcproc_ret_imm32(x86state* ps, x86opcodefamily* popcf, uint8_t opc);
void opcproc_ret(x86state* ps, x86opcodefamily* popcf, uint8_t opc);

void opcproc_jmp(x86state* ps, x86opcodefamily* popcf, uint8_t opc);

void opcproc_mov_imm8(x86state* ps, x86opcodefamily* popcf, uint8_t opc);
void opcproc_mov_imm32(x86state* ps, x86opcodefamily* popcf, uint8_t opc);

//complement/clear/set carry flag
void opcproc_cmc(x86state* ps, x86opcodefamily* popcf, uint8_t opc);
void opcproc_clc(x86state* ps, x86opcodefamily* popcf, uint8_t opc);
void opcproc_stc(x86state* ps, x86opcodefamily* popcf, uint8_t opc);

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

void opcproc_unsupported(x86state* ps, x86opcodefamily* popcf, uint8_t opc)
{
  
}

void opcproc_alu(x86state* ps, x86opcodefamily* popcf, uint8_t opc)
{
  
}

int32_t opcprocalu_add(int32_t a, int32_t b, x86state* ps)
{
  
}

int32_t opcprocalu_or(int32_t a, int32_t b, x86state* ps)
{
  
}

int32_t opcprocalu_adc(int32_t a, int32_t b, x86state* ps)
{
  
}

int32_t opcprocalu_sbb(int32_t a, int32_t b, x86state* ps)
{
  
}

int32_t opcprocalu_and(int32_t a, int32_t b, x86state* ps)
{
  
}

int32_t opcprocalu_sub(int32_t a, int32_t b, x86state* ps)
{
  
}

int32_t opcprocalu_xor(int32_t a, int32_t b, x86state* ps)
{
  
}

int32_t opcprocalu_cmp(int32_t a, int32_t b, x86state* ps)
{
  
}

void opcproc_inc(x86state* ps, x86opcodefamily* popcf, uint8_t opc)
{
  
}

void opcproc_dec(x86state* ps, x86opcodefamily* popcf, uint8_t opc)
{
  
}

void opcproc_push(x86state* ps, x86opcodefamily* popcf, uint8_t opc)
{
  
}

void opcproc_pop(x86state* ps, x86opcodefamily* popcf, uint8_t opc)
{
  
}

void opcproc_jrc8(x86state* ps, x86opcodefamily* popcf, uint8_t opc)
{

}

void opcproc_grp1(x86state* ps, x86opcodefamily* popcf, uint8_t opc)
{
  
}

void opcproc_xchg(x86state* ps, x86opcodefamily* popcf, uint8_t opc)
{
  
}

void opcproc_mov(x86state* ps, x86opcodefamily* popcf, uint8_t opc)
{
  
}

//pop a general operand (POP Ev)
void opcproc_pop_general(x86state* ps, x86opcodefamily* popcf, uint8_t opc)
{
  
}

void opcproc_call(x86state* ps, x86opcodefamily* popcf, uint8_t opc)
{
  
}

void opcproc_pushf(x86state* ps, x86opcodefamily* popcf, uint8_t opc)
{
  
}

void opcproc_popf(x86state* ps, x86opcodefamily* popcf, uint8_t opc)
{
  
}

void opcproc_sahf(x86state* ps, x86opcodefamily* popcf, uint8_t opc)
{
  
}

void opcproc_lahf(x86state* ps, x86opcodefamily* popcf, uint8_t opc)
{
  
}

void opcproc_xchgAX(x86state* ps, x86opcodefamily* popcf, uint8_t opc)
{
  
}

void opcproc_mov_imm8(x86state* ps, x86opcodefamily* popcf, uint8_t opc)
{
  
}

void opcproc_mov_imm32(x86state* ps, x86opcodefamily* popcf, uint8_t opc)
{
  
}

void opcproc_ret_imm32(x86state* ps, x86opcodefamily* popcf, uint8_t opc)
{
  
}

void opcproc_ret(x86state* ps, x86opcodefamily* popcf, uint8_t opc)
{
  
}

void opcproc_grp2(x86state* ps, x86opcodefamily* popcf, uint8_t opc)
{
  
}

void opcproc_jmp(x86state* ps, x86opcodefamily* popcf, uint8_t opc)
{
  
}

void opcproc_mov_general_imm8(x86state* ps, x86opcodefamily* popcf, uint8_t opc)
{
  
}

void opcproc_mov_general_imm32(x86state* ps, x86opcodefamily* popcf, uint8_t opc)
{
  
}

void opcproc_cmc(x86state* ps, x86opcodefamily* popcf, uint8_t opc)
{
  
}

void opcproc_clc(x86state* ps, x86opcodefamily* popcf, uint8_t opc)
{
  
}

void opcproc_stc(x86state* ps, x86opcodefamily* popcf, uint8_t opc)
{
  
}
