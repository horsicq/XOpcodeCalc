// Copyright (c) 2019-2020 hors<horsicq@gmail.com>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//
#ifndef ASM_DEFS_H
#define ASM_DEFS_H

#include "asm/asm.h"

namespace ASM_DEF
{
struct VALUE_RECORD
{
    const char *pszRegName;
    unsigned long long nMaxValue;
};

enum OP
{
    OP_UNKNOWN=0,
    OP_ADD,
    OP_SUB,
    OP_ADC,
    OP_SBB,
    OP_AND,
    OP_XOR,
    OP_OR,
    OP_INC,
    OP_DEC,
    OP_TEST,
    OP_CMP,
    OP_MUL,
    OP_IMUL,
    OP_DIV,
    OP_IDIV,
    OP_NOT,
    OP_NEG,
    OP_SHR,
    OP_SHL,
    OP_SAR,
    OP_ROL,
    OP_ROR,
    OP_RCL,
    OP_RCR,
    OP_CPUID,
    OP_BSWAP,
    OP_BT,
    OP_BSF,
    OP_BTS,
    OP_BTR,
    OP_BTC,
    OP_BSR
};

const XVALUE CF=0x0001;
const XVALUE PF=0x0004;
const XVALUE AF=0x0010;
const XVALUE ZF=0x0040;
const XVALUE SF=0x0080;
const XVALUE OF=0x0800;

const unsigned int          LIM32=0xFFFFFFFF;
const unsigned long long    LIM64=0xFFFFFFFFFFFFFFFF;

typedef void (*_asm_func)(RECDATA *pRecData);

struct OPCODE_RECORD
{
    OP opcode;
    _asm_func asm_func;
    const char *pszName;
    VALUE_RECORD vrOperand[2];
    VALUE_RECORD vrResult[4];
    const char *pszExample;
};

#ifdef OPCODE32
const OPCODE_RECORD asm_twooperands[]=
{
{OP_ADD,    &op_add,    "add",  {{"EAX",    LIM32}, {"ECX",     LIM32}},{{"EAX",    LIM32}, {"",    0},     {"",    0},     {"",    0}},    "ADD EAX,ECX"},
{OP_SUB,    &op_sub,    "sub",  {{"EAX",    LIM32}, {"ECX",     LIM32}},{{"EAX",    LIM32}, {"",    0},     {"",    0},     {"",    0}},    "SUB EAX,ECX"},
{OP_ADC,    &op_adc,    "adc",  {{"EAX",    LIM32}, {"ECX",     LIM32}},{{"EAX",    LIM32}, {"",    0},     {"",    0},     {"",    0}},    "ADC EAX,ECX"},
{OP_SBB,    &op_sbb,    "sbb",  {{"EAX",    LIM32}, {"ECX",     LIM32}},{{"EAX",    LIM32}, {"",    0},     {"",    0},     {"",    0}},    "SBB EAX,ECX"},
{OP_AND,    &op_and,    "and",  {{"EAX",    LIM32}, {"ECX",     LIM32}},{{"EAX",    LIM32}, {"",    0},     {"",    0},     {"",    0}},    "AND EAX,ECX"},
{OP_XOR,    &op_xor,    "xor",  {{"EAX",    LIM32}, {"ECX",     LIM32}},{{"EAX",    LIM32}, {"",    0},     {"",    0},     {"",    0}},    "XOR EAX,ECX"},
{OP_OR,     &op_or,     "or",   {{"EAX",    LIM32}, {"ECX",     LIM32}},{{"EAX",    LIM32}, {"",    0},     {"",    0},     {"",    0}},    "OR EAX,ECX"},
{OP_TEST,   &op_test,   "test", {{"EAX",    LIM32}, {"ECX",     LIM32}},{{"",       0},     {"",    0},     {"",    0},     {"",    0}},    "TEST EAX,ECX"},
{OP_CMP,    &op_cmp,    "cmp",  {{"EAX",    LIM32}, {"ECX",     LIM32}},{{"",       0},     {"",    0},     {"",    0},     {"",    0}},    "CMP EAX,ECX"},
};
const OPCODE_RECORD asm_oneoperand[]=
{
{OP_INC,    &op_inc,    "inc",  {{"EAX",    LIM32}, {"",        0}},    {{"EAX",    LIM32}, {"",    0},     {"",    0},     {"",    0}},    "INC EAX"},
{OP_DEC,    &op_dec,    "dec",  {{"EAX",    LIM32}, {"",        0}},    {{"EAX",    LIM32}, {"",    0},     {"",    0},     {"",    0}},    "DEC EAX"},
{OP_NOT,    &op_not,    "not",  {{"EAX",    LIM32}, {"",        0}},    {{"EAX",    LIM32}, {"",    0},     {"",    0},     {"",    0}},    "NOT EAX"},
{OP_NEG,    &op_neg,    "neg",  {{"EAX",    LIM32}, {"",        0}},    {{"EAX",    LIM32}, {"",    0},     {"",    0},     {"",    0}},    "NEG EAX"},
{OP_BSWAP,  &op_bswap,  "bswap",{{"EAX",    LIM32}, {"",        0}},    {{"EAX",    LIM32}, {"",    0},     {"",    0},     {"",    0}},    "BSWAP EAX"},
};
const OPCODE_RECORD asm_muldiv[]=
{
{OP_MUL,    &op_mul,    "mul",  {{"EAX",    LIM32}, {"ECX",     LIM32}},{{"EAX",    LIM32}, {"EDX", LIM32}, {"",    0},     {"",    0}},    "MUL ECX"},
{OP_IMUL,   &op_imul,   "imul", {{"EAX",    LIM32}, {"ECX",     LIM32}},{{"EAX",    LIM32}, {"EDX", LIM32}, {"",    0},     {"",    0}},    "IMUL ECX"},
{OP_DIV,    &op_div,    "div",  {{"EAX",    LIM32}, {"ECX",     LIM32}},{{"EAX",    LIM32}, {"EDX", LIM32}, {"",    0},     {"",    0}},    "DIV ECX"},
{OP_IDIV,   &op_idiv,   "idiv", {{"EAX",    LIM32}, {"ECX",     LIM32}},{{"EAX",    LIM32}, {"EDX", LIM32}, {"",    0},     {"",    0}},    "IDIV ECX"},
};
const OPCODE_RECORD asm_shift[]=
{
{OP_SHR,    &op_shr,    "shr",  {{"EAX",    LIM32}, {"CL",      31}},   {{"EAX",    LIM32}, {"",    0},     {"",    0},     {"",    0}},    "SHR EAX,CL"},
{OP_SHL,    &op_shl,    "shl",  {{"EAX",    LIM32}, {"CL",      31}},   {{"EAX",    LIM32}, {"",    0},     {"",    0},     {"",    0}},    "SHL EAX,CL"},
{OP_SAR,    &op_sar,    "sar",  {{"EAX",    LIM32}, {"CL",      31}},   {{"EAX",    LIM32}, {"",    0},     {"",    0},     {"",    0}},    "SAR EAX,CL"},
{OP_ROL,    &op_rol,    "rol",  {{"EAX",    LIM32}, {"CL",      31}},   {{"EAX",    LIM32}, {"",    0},     {"",    0},     {"",    0}},    "ROL EAX,CL"},
{OP_ROR,    &op_ror,    "ror",  {{"EAX",    LIM32}, {"CL",      31}},   {{"EAX",    LIM32}, {"",    0},     {"",    0},     {"",    0}},    "ROR EAX,CL"},
{OP_RCL,    &op_rcl,    "rcl",  {{"EAX",    LIM32}, {"CL",      31}},   {{"EAX",    LIM32}, {"",    0},     {"",    0},     {"",    0}},    "RCL EAX,CL"},
{OP_RCR,    &op_rcr,    "rcr",  {{"EAX",    LIM32}, {"CL",      31}},   {{"EAX",    LIM32}, {"",    0},     {"",    0},     {"",    0}},    "RCR EAX,CL"},
};
const OPCODE_RECORD asm_bits[]=
{
{OP_BT,     &op_bt,     "bt",   {{"EAX",    LIM32}, {"ECX",     LIM32}},{{"EAX",    LIM32}, {"",    0},     {"",    0},     {"",    0}},    "BT EAX,ECX"},
{OP_BTS,    &op_bts,    "bts",  {{"EAX",    LIM32}, {"ECX",     LIM32}},{{"EAX",    LIM32}, {"",    0},     {"",    0},     {"",    0}},    "BTS EAX,ECX"},
{OP_BTR,    &op_btr,    "btr",  {{"EAX",    LIM32}, {"ECX",     LIM32}},{{"EAX",    LIM32}, {"",    0},     {"",    0},     {"",    0}},    "BTR EAX,ECX"},
{OP_BTC,    &op_btc,    "btc",  {{"EAX",    LIM32}, {"ECX",     LIM32}},{{"EAX",    LIM32}, {"",    0},     {"",    0},     {"",    0}},    "BTC EAX,ECX"},
{OP_BSF,    &op_bsf,    "bsf",  {{"EAX",    LIM32}, {"ECX",     LIM32}},{{"EAX",    LIM32}, {"",    0},     {"",    0},     {"",    0}},    "BSF EAX,ECX"},
{OP_BSR,    &op_bsr,    "bsr",  {{"EAX",    LIM32}, {"ECX",     LIM32}},{{"EAX",    LIM32}, {"",    0},     {"",    0},     {"",    0}},    "BSR EAX,ECX"},
};
const OPCODE_RECORD asm_special[]=
{
{OP_CPUID,  &op_cpuid,  "cpuid",{{"EAX",    LIM32}, {"ECX",     LIM32}},{{"EAX",    LIM32}, {"EDX", LIM32}, {"ECX", LIM32}, {"EBX", LIM32}},"CPUID"},
};
#else
const OPCODE_RECORD asm_twooperands[]=
{
{OP_ADD,    &op_add,    "add",  {{"RAX",    LIM64}, {"RCX",     LIM64}},{{"RAX",    LIM64}, {"",    0},     {"",    0},     {"",    0}},    "ADD RAX,RCX"},
{OP_SUB,    &op_sub,    "sub",  {{"RAX",    LIM64}, {"RCX",     LIM64}},{{"RAX",    LIM64}, {"",    0},     {"",    0},     {"",    0}},    "SUB RAX,RCX"},
{OP_ADC,    &op_adc,    "adc",  {{"RAX",    LIM64}, {"RCX",     LIM64}},{{"RAX",    LIM64}, {"",    0},     {"",    0},     {"",    0}},    "ADC RAX,RCX"},
{OP_SBB,    &op_sbb,    "sbb",  {{"RAX",    LIM64}, {"RCX",     LIM64}},{{"RAX",    LIM64}, {"",    0},     {"",    0},     {"",    0}},    "SBB RAX,RCX"},
{OP_AND,    &op_and,    "and",  {{"RAX",    LIM64}, {"RCX",     LIM64}},{{"RAX",    LIM64}, {"",    0},     {"",    0},     {"",    0}},    "AND RAX,RCX"},
{OP_XOR,    &op_xor,    "xor",  {{"RAX",    LIM64}, {"RCX",     LIM64}},{{"RAX",    LIM64}, {"",    0},     {"",    0},     {"",    0}},    "XOR RAX,RCX"},
{OP_OR,     &op_or,     "or",   {{"RAX",    LIM64}, {"RCX",     LIM64}},{{"RAX",    LIM64}, {"",    0},     {"",    0},     {"",    0}},    "OR RAX,RCX"},
{OP_TEST,   &op_test,   "test", {{"RAX",    LIM64}, {"RCX",     LIM64}},{{"",       0},     {"",    0},     {"",    0},     {"",    0}},    "TEST RAX,RCX"},
{OP_CMP,    &op_cmp,    "cmp",  {{"RAX",    LIM64}, {"RCX",     LIM64}},{{"",       0},     {"",    0},     {"",    0},     {"",    0}},    "CMP RAX,RCX"},
};
const OPCODE_RECORD asm_oneoperand[]=
{
{OP_INC,    &op_inc,    "inc",  {{"RAX",    LIM64}, {"",        0}},    {{"RAX",    LIM64}, {"",    0},     {"",    0},     {"",    0}},    "INC RAX"},
{OP_DEC,    &op_dec,    "dec",  {{"RAX",    LIM64}, {"",        0}},    {{"RAX",    LIM64}, {"",    0},     {"",    0},     {"",    0}},    "DEC RAX"},
{OP_NOT,    &op_not,    "not",  {{"RAX",    LIM64}, {"",        0}},    {{"RAX",    LIM64}, {"",    0},     {"",    0},     {"",    0}},    "NOT RAX"},
{OP_NEG,    &op_neg,    "neg",  {{"RAX",    LIM64}, {"",        0}},    {{"RAX",    LIM64}, {"",    0},     {"",    0},     {"",    0}},    "NEG RAX"},
{OP_BSWAP,  &op_bswap,  "bswap",{{"RAX",    LIM32}, {"",        0}},    {{"RAX",    LIM32}, {"",    0},     {"",    0},     {"",    0}},    "BSWAP RAX"},
};
const OPCODE_RECORD asm_muldiv[]=
{
{OP_MUL,    &op_mul,    "mul",  {{"RAX",    LIM64}, {"RCX",     LIM64}},{{"RAX",    LIM64}, {"RDX", LIM64}, {"",    0},     {"",    0}},    "MUL RCX"},
{OP_IMUL,   &op_imul,   "imul", {{"RAX",    LIM64}, {"RCX",     LIM64}},{{"RAX",    LIM64}, {"RDX", LIM64}, {"",    0},     {"",    0}},    "IMUL RCX"},
{OP_DIV,    &op_div,    "div",  {{"RAX",    LIM64}, {"RCX",     LIM64}},{{"RAX",    LIM64}, {"RDX", LIM64}, {"",    0},     {"",    0}},    "DIV RCX"},
{OP_IDIV,   &op_idiv,   "idiv", {{"RAX",    LIM64}, {"RCX",     LIM64}},{{"RAX",    LIM64}, {"RDX", LIM64}, {"",    0},     {"",    0}},    "IDIV RCX"},
};
const OPCODE_RECORD asm_shift[]=
{
{OP_SHR,    &op_shr,    "shr",  {{"RAX",    LIM64}, {"CL",      63}},   {{"RAX",    LIM64}, {"",    0},     {"",    0},     {"",    0}},    "SHR RAX,CL"},
{OP_SHL,    &op_shl,    "shl",  {{"RAX",    LIM64}, {"CL",      63}},   {{"RAX",    LIM64}, {"",    0},     {"",    0},     {"",    0}},    "SHL RAX,CL"},
{OP_SAR,    &op_sar,    "sar",  {{"RAX",    LIM64}, {"CL",      63}},   {{"RAX",    LIM64}, {"",    0},     {"",    0},     {"",    0}},    "SAR RAX,CL"},
{OP_ROL,    &op_rol,    "rol",  {{"RAX",    LIM64}, {"CL",      63}},   {{"RAX",    LIM64}, {"",    0},     {"",    0},     {"",    0}},    "ROL RAX,CL"},
{OP_ROR,    &op_ror,    "ror",  {{"RAX",    LIM64}, {"CL",      63}},   {{"RAX",    LIM64}, {"",    0},     {"",    0},     {"",    0}},    "ROR RAX,CL"},
{OP_RCL,    &op_rcl,    "rcl",  {{"RAX",    LIM64}, {"CL",      63}},   {{"RAX",    LIM64}, {"",    0},     {"",    0},     {"",    0}},    "RCL RAX,CL"},
{OP_RCR,    &op_rcr,    "rcr",  {{"RAX",    LIM64}, {"CL",      63}},   {{"RAX",    LIM64}, {"",    0},     {"",    0},     {"",    0}},    "RCR RAX,CL"},
};
const OPCODE_RECORD asm_bits[]=
{
{OP_BT,     &op_bt,     "bt",   {{"RAX",    LIM64}, {"RCX",     LIM64}},{{"EAX",    LIM64}, {"",    0},     {"",    0},     {"",    0}},    "BT RAX,RCX"},
{OP_BTS,    &op_bts,    "bts",  {{"RAX",    LIM64}, {"RCX",     LIM64}},{{"RAX",    LIM64}, {"",    0},     {"",    0},     {"",    0}},    "BTS RAX,RCX"},
{OP_BTR,    &op_btr,    "btr",  {{"RAX",    LIM64}, {"RCX",     LIM64}},{{"RAX",    LIM64}, {"",    0},     {"",    0},     {"",    0}},    "BTR RAX,RCX"},
{OP_BTC,    &op_btc,    "btc",  {{"RAX",    LIM64}, {"RCX",     LIM64}},{{"RAX",    LIM64}, {"",    0},     {"",    0},     {"",    0}},    "BTC RAX,RCX"},
{OP_BSF,    &op_bsf,    "bsf",  {{"RAX",    LIM64}, {"RCX",     LIM64}},{{"RAX",    LIM64}, {"",    0},     {"",    0},     {"",    0}},    "BSF RAX,RCX"},
{OP_BSR,    &op_bsr,    "bsr",  {{"RAX",    LIM64}, {"RCX",     LIM64}},{{"RAX",    LIM64}, {"",    0},     {"",    0},     {"",    0}},    "BSR RAX,RCX"},
};

const OPCODE_RECORD asm_special[]=
{
{OP_CPUID,  &op_cpuid,  "cpuid",{{"EAX",    LIM32}, {"ECX",     LIM32}},{{"EAX",    LIM32}, {"EDX", LIM32}, {"ECX", LIM32}, {"EBX", LIM32}},"CPUID"},
};
#endif
}

#endif // ASM_DEFS_H
