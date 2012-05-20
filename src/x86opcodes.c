#include <stdlib.h>
#include <stdio.h>

#include "x86emu.h"

void opcproc_unsupported(x86state* ps, x86opcodefamily* popcf, uint8_t opc);

void opcproc_alu(x86state* ps, x86opcodefamily* popcf, uint8_t opc);
int32_t opcprocalu_add(int32_t a, int32_t b, x86flags* pflags);
int32_t opcprocalu_or(int32_t a, int32_t b, x86flags* pflags);
int32_t opcprocalu_adc(int32_t a, int32_t b, x86flags* pflags);
int32_t opcprocalu_sbb(int32_t a, int32_t b, x86flags* pflags);
int32_t opcprocalu_and(int32_t a, int32_t b, x86flags* pflags);
int32_t opcprocalu_sub(int32_t a, int32_t b, x86flags* pflags);
int32_t opcprocalu_xor(int32_t a, int32_t b, x86flags* pflags);
int32_t opcprocalu_cmp(int32_t a, int32_t b, x86flags* pflags);

void opcproc_inc(x86state* ps, x86opcodefamily* popcf, uint8_t opc);
void opcproc_dec(x86state* ps, x86opcodefamily* popcf, uint8_t opc);
void opcproc_push(x86state* ps, x86opcodefamily* popcf, uint8_t opc);
void opcproc_pop(x86state* ps, x86opcodefamily* popcf, uint8_t opc);

void opcproc_jrc8(x86state* ps, x86opcodefamily* popcf, uint8_t opc);


#define OPCF_UNSUPPORTED(opstr,msg) {opcproc_unsupported, { .unsupported = {opstr,msg}}}

#define OPCF_ALU(op,c) {opcproc_alu, { .alu = {opcprocalu_ ## op, c, 0, 0, 0}}}, \
                       {opcproc_alu, { .alu = {opcprocalu_ ## op, c, 1, 0, 0}}}, \
                       {opcproc_alu, { .alu = {opcprocalu_ ## op, c, 0, 1, 0}}}, \
                       {opcproc_alu, { .alu = {opcprocalu_ ## op, c, 1, 1, 0}}}, \
                       {opcproc_alu, { .alu = {opcprocalu_ ## op, c, 0, 0, 1}}}, \
                       {opcproc_alu, { .alu = {opcprocalu_ ## op, c, 1, 0, 1}}}
                       
#define OPCF_B1(op) {opcproc_ ## op, { .b1 = { REG_EAX }}}, \
                    {opcproc_ ## op, { .b1 = { REG_ECX }}}, \
                    {opcproc_ ## op, { .b1 = { REG_EDX }}}, \
                    {opcproc_ ## op, { .b1 = { REG_EBX }}}, \
                    {opcproc_ ## op, { .b1 = { REG_ESP }}}, \
                    {opcproc_ ## op, { .b1 = { REG_EBP }}}, \
                    {opcproc_ ## op, { .b1 = { REG_ESI }}}, \
                    {opcproc_ ## op, { .b1 = { REG_EDI }}}
                    
#define OPCF_JRC8(cond) {opcproc_jrc8, { .jrc8 = { cond }}}

x86opcodefamily opc_families[256] = {
  OPCF_ALU(add,0),
  OPCF_UNSUPPORTED("PUSH ES","segment register push unsupported"),
  OPCF_UNSUPPORTED("POP ES","segment register pop unsupported"),
  
  OPCF_ALU(or,0),
  OPCF_UNSUPPORTED("PUSH CS","segment register push unsupported"),
  OPCF_UNSUPPORTED("[0F]","two-byte instructions unsupported"),
  
  OPCF_ALU(adc,1),
  OPCF_UNSUPPORTED("PUSH SS","segment register push unsupported"),
  OPCF_UNSUPPORTED("POP SS","segment register pop unsupported"),
  
  OPCF_ALU(sbb,1),
  OPCF_UNSUPPORTED("PUSH DS","segment register push unsupported"),
  OPCF_UNSUPPORTED("POP DS","segment register pop unsupported"),
  
  OPCF_ALU(and,0),
  OPCF_UNSUPPORTED("[ES:]","segment override prefix unsupported"),
  OPCF_UNSUPPORTED("DAA AL","decimal adjust instructions unsupported"),
  
  OPCF_ALU(sub,0),
  OPCF_UNSUPPORTED("[CS:]","segment override prefix unsupported"),
  OPCF_UNSUPPORTED("DAS AL","decimal adjust instructions unsupported"),
  
  OPCF_ALU(xor,0),
  OPCF_UNSUPPORTED("[SS:]","segment override prefix unsupported"),
  OPCF_UNSUPPORTED("AAA AL","ASCII adjust instructions unsupported"),
  
  OPCF_ALU(cmp,0),
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
};

#undef OPCF_UNSUPPORTED
#undef OPCF_ALU
#undef OPCF_B1

void opcproc_unsupported(x86state* ps, x86opcodefamily* popcf, uint8_t opc)
{
  
}

void opcproc_alu(x86state* ps, x86opcodefamily* popcf, uint8_t opc)
{
  
}

int32_t opcprocalu_add(int32_t a, int32_t b, x86flags* pflags)
{
  
}

int32_t opcprocalu_or(int32_t a, int32_t b, x86flags* pflags)
{
  
}

int32_t opcprocalu_adc(int32_t a, int32_t b, x86flags* pflags)
{
  
}

int32_t opcprocalu_sbb(int32_t a, int32_t b, x86flags* pflags)
{
  
}

int32_t opcprocalu_and(int32_t a, int32_t b, x86flags* pflags)
{
  
}

int32_t opcprocalu_sub(int32_t a, int32_t b, x86flags* pflags)
{
  
}

int32_t opcprocalu_xor(int32_t a, int32_t b, x86flags* pflags)
{
  
}

int32_t opcprocalu_cmp(int32_t a, int32_t b, x86flags* pflags)
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



