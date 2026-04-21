/* Copyright (c) 2019-2026 hors<horsicq@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#ifndef ASM_DEFS_H
#define ASM_DEFS_H

#include "asm/asm.h"

namespace ASM_DEF {
struct VALUE_RECORD {
    const char *pszRegName;
    unsigned long long nMaxValue;
};

enum OP {
    OP_UNKNOWN = 0,
    OP_ADD,
    OP_SUB,
    OP_ADC,
    OP_SBB,
    OP_AND,
    OP_XOR,
    OP_OR,
    OP_MOV,
    OP_XCHG,
    OP_INC,
    OP_DEC,
    OP_TEST,
    OP_CMP,
    OP_CMPXCHG,
    OP_MUL,
    OP_IMUL,
    OP_DIV,
    OP_IDIV,
    OP_NOT,
    OP_NEG,
    OP_SHR,
    OP_SHL,
    OP_SAR,
    OP_SHLD,
    OP_SHRD,
    OP_ROL,
    OP_ROR,
    OP_RCL,
    OP_RCR,
    OP_CPUID,
    OP_RDTSC,
    OP_CLC,
    OP_STC,
    OP_CMC,
    OP_LAHF,
    OP_SAHF,
    OP_BSWAP,
    OP_BT,
    OP_BSF,
    OP_BTS,
    OP_BTR,
    OP_BTC,
    OP_BSR,
    OP_SETC,
    OP_SETNC,
    OP_SETZ,
    OP_SETNZ,
    OP_SETS,
    OP_SETNS,
    OP_SETO,
    OP_SETNO,
    OP_SETP,
    OP_SETNP,
    OP_CMOVC,
    OP_CMOVNC,
    OP_CMOVZ,
    OP_CMOVNZ,
    OP_CMOVS,
    OP_CMOVNS,
    OP_CMOVO,
    OP_CMOVNO,
    OP_CMOVP,
    OP_CMOVNP,
    OP_XADD,
    OP_MOVZX_R8,
    OP_MOVZX_R16,
    OP_MOVSX_R8,
    OP_MOVSX_R16,
#ifdef OPCODE64
    OP_MOVSXD,
    OP_CDQE,
    OP_CQO,
#else
    OP_CBW,
    OP_CWDE,
    OP_CWD,
    OP_CDQ,
#endif
#ifndef OPCODE64
    OP_DAA,
    OP_DAS,
    OP_AAA,
    OP_AAS,
    OP_AAM,
    OP_AAD
#endif
};

const XVALUE CF = 0x0001;
const XVALUE PF = 0x0004;
const XVALUE AF = 0x0010;
const XVALUE ZF = 0x0040;
const XVALUE SF = 0x0080;
const XVALUE OF = 0x0800;

const unsigned char LIMBS32 = 0x31;
const unsigned char LIMBS64 = 0x63;
const unsigned char LIM8 = 0xFF;
const unsigned short LIM16 = 0xFFFF;
const unsigned int LIM32 = 0xFFFFFFFF;
const unsigned long long LIM64 = 0xFFFFFFFFFFFFFFFF;

typedef void (*_asm_func)(RECDATA *pRecData);

struct OPCODE_RECORD {
    OP opcode;
    _asm_func asm_func;
    const char *pszName;
    VALUE_RECORD vrOperand[4];
    VALUE_RECORD vrResult[4];
    const char *pszExample;
};

#ifdef OPCODE32
const OPCODE_RECORD asm_twooperands[] = {
    {OP_ADD, &op_add, "add", {{"EAX", LIM32}, {"ECX", LIM32}, {"", 0}, {"", 0}}, {{"EAX", LIM32}, {"", 0}, {"", 0}, {"", 0}}, "ADD EAX,ECX"},
    {OP_SUB, &op_sub, "sub", {{"EAX", LIM32}, {"ECX", LIM32}, {"", 0}, {"", 0}}, {{"EAX", LIM32}, {"", 0}, {"", 0}, {"", 0}}, "SUB EAX,ECX"},
    {OP_ADC, &op_adc, "adc", {{"EAX", LIM32}, {"ECX", LIM32}, {"", 0}, {"", 0}}, {{"EAX", LIM32}, {"", 0}, {"", 0}, {"", 0}}, "ADC EAX,ECX"},
    {OP_SBB, &op_sbb, "sbb", {{"EAX", LIM32}, {"ECX", LIM32}, {"", 0}, {"", 0}}, {{"EAX", LIM32}, {"", 0}, {"", 0}, {"", 0}}, "SBB EAX,ECX"},
    {OP_AND, &op_and, "and", {{"EAX", LIM32}, {"ECX", LIM32}, {"", 0}, {"", 0}}, {{"EAX", LIM32}, {"", 0}, {"", 0}, {"", 0}}, "AND EAX,ECX"},
    {OP_XOR, &op_xor, "xor", {{"EAX", LIM32}, {"ECX", LIM32}, {"", 0}, {"", 0}}, {{"EAX", LIM32}, {"", 0}, {"", 0}, {"", 0}}, "XOR EAX,ECX"},
    {OP_OR, &op_or, "or", {{"EAX", LIM32}, {"ECX", LIM32}, {"", 0}, {"", 0}}, {{"EAX", LIM32}, {"", 0}, {"", 0}, {"", 0}}, "OR EAX,ECX"},
    {OP_MOV, &op_mov, "mov", {{"EAX", LIM32}, {"ECX", LIM32}, {"", 0}, {"", 0}}, {{"EAX", LIM32}, {"", 0}, {"", 0}, {"", 0}}, "MOV EAX,ECX"},
    {OP_XCHG, &op_xchg, "xchg", {{"EAX", LIM32}, {"ECX", LIM32}, {"", 0}, {"", 0}}, {{"EAX", LIM32}, {"ECX", LIM32}, {"", 0}, {"", 0}}, "XCHG EAX,ECX"},
    {OP_TEST, &op_test, "test", {{"EAX", LIM32}, {"ECX", LIM32}, {"", 0}, {"", 0}}, {{"", 0}, {"", 0}, {"", 0}, {"", 0}}, "TEST EAX,ECX"},
    {OP_CMP, &op_cmp, "cmp", {{"EAX", LIM32}, {"ECX", LIM32}, {"", 0}, {"", 0}}, {{"", 0}, {"", 0}, {"", 0}, {"", 0}}, "CMP EAX,ECX"},
    {OP_XADD, &op_xadd, "xadd", {{"EAX", LIM32}, {"ECX", LIM32}, {"", 0}, {"", 0}}, {{"EAX", LIM32}, {"ECX", LIM32}, {"", 0}, {"", 0}}, "XADD EAX,ECX"},
    {OP_CMOVC, &op_cmovc, "cmovc", {{"EAX", LIM32}, {"ECX", LIM32}, {"", 0}, {"", 0}}, {{"EAX", LIM32}, {"", 0}, {"", 0}, {"", 0}}, "CMOVC EAX,ECX"},
    {OP_CMOVNC, &op_cmovnc, "cmovnc", {{"EAX", LIM32}, {"ECX", LIM32}, {"", 0}, {"", 0}}, {{"EAX", LIM32}, {"", 0}, {"", 0}, {"", 0}}, "CMOVNC EAX,ECX"},
    {OP_CMOVZ, &op_cmovz, "cmovz", {{"EAX", LIM32}, {"ECX", LIM32}, {"", 0}, {"", 0}}, {{"EAX", LIM32}, {"", 0}, {"", 0}, {"", 0}}, "CMOVZ EAX,ECX"},
    {OP_CMOVNZ, &op_cmovnz, "cmovnz", {{"EAX", LIM32}, {"ECX", LIM32}, {"", 0}, {"", 0}}, {{"EAX", LIM32}, {"", 0}, {"", 0}, {"", 0}}, "CMOVNZ EAX,ECX"},
    {OP_CMOVS, &op_cmovs, "cmovs", {{"EAX", LIM32}, {"ECX", LIM32}, {"", 0}, {"", 0}}, {{"EAX", LIM32}, {"", 0}, {"", 0}, {"", 0}}, "CMOVS EAX,ECX"},
    {OP_CMOVNS, &op_cmovns, "cmovns", {{"EAX", LIM32}, {"ECX", LIM32}, {"", 0}, {"", 0}}, {{"EAX", LIM32}, {"", 0}, {"", 0}, {"", 0}}, "CMOVNS EAX,ECX"},
    {OP_CMOVO, &op_cmovo, "cmovo", {{"EAX", LIM32}, {"ECX", LIM32}, {"", 0}, {"", 0}}, {{"EAX", LIM32}, {"", 0}, {"", 0}, {"", 0}}, "CMOVO EAX,ECX"},
    {OP_CMOVNO, &op_cmovno, "cmovno", {{"EAX", LIM32}, {"ECX", LIM32}, {"", 0}, {"", 0}}, {{"EAX", LIM32}, {"", 0}, {"", 0}, {"", 0}}, "CMOVNO EAX,ECX"},
    {OP_CMOVP, &op_cmovp, "cmovp", {{"EAX", LIM32}, {"ECX", LIM32}, {"", 0}, {"", 0}}, {{"EAX", LIM32}, {"", 0}, {"", 0}, {"", 0}}, "CMOVP EAX,ECX"},
    {OP_CMOVNP, &op_cmovnp, "cmovnp", {{"EAX", LIM32}, {"ECX", LIM32}, {"", 0}, {"", 0}}, {{"EAX", LIM32}, {"", 0}, {"", 0}, {"", 0}}, "CMOVNP EAX,ECX"},
    {OP_MOVZX_R8, &op_movzx_r8, "movzx r32,r8", {{"EAX", LIM32}, {"CL", LIM8}, {"", 0}, {"", 0}}, {{"EAX", LIM32}, {"", 0}, {"", 0}, {"", 0}}, "MOVZX EAX,CL"},
    {OP_MOVZX_R16, &op_movzx_r16, "movzx r32,r16", {{"EAX", LIM32}, {"CX", LIM16}, {"", 0}, {"", 0}}, {{"EAX", LIM32}, {"", 0}, {"", 0}, {"", 0}}, "MOVZX EAX,CX"},
    {OP_MOVSX_R8, &op_movsx_r8, "movsx r32,r8", {{"EAX", LIM32}, {"CL", LIM8}, {"", 0}, {"", 0}}, {{"EAX", LIM32}, {"", 0}, {"", 0}, {"", 0}}, "MOVSX EAX,CL"},
    {OP_MOVSX_R16, &op_movsx_r16, "movsx r32,r16", {{"EAX", LIM32}, {"CX", LIM16}, {"", 0}, {"", 0}}, {{"EAX", LIM32}, {"", 0}, {"", 0}, {"", 0}}, "MOVSX EAX,CX"},
};
const OPCODE_RECORD asm_oneoperand[] = {
    {OP_INC, &op_inc, "inc", {{"EAX", LIM32}, {"", 0}, {"", 0}, {"", 0}}, {{"EAX", LIM32}, {"", 0}, {"", 0}, {"", 0}}, "INC EAX"},
    {OP_DEC, &op_dec, "dec", {{"EAX", LIM32}, {"", 0}, {"", 0}, {"", 0}}, {{"EAX", LIM32}, {"", 0}, {"", 0}, {"", 0}}, "DEC EAX"},
    {OP_NOT, &op_not, "not", {{"EAX", LIM32}, {"", 0}, {"", 0}, {"", 0}}, {{"EAX", LIM32}, {"", 0}, {"", 0}, {"", 0}}, "NOT EAX"},
    {OP_NEG, &op_neg, "neg", {{"EAX", LIM32}, {"", 0}, {"", 0}, {"", 0}}, {{"EAX", LIM32}, {"", 0}, {"", 0}, {"", 0}}, "NEG EAX"},
    {OP_BSWAP, &op_bswap, "bswap", {{"EAX", LIM32}, {"", 0}, {"", 0}, {"", 0}}, {{"EAX", LIM32}, {"", 0}, {"", 0}, {"", 0}}, "BSWAP EAX"},
    {OP_LAHF, &op_lahf, "lahf", {{"EAX", LIM32}, {"", 0}, {"", 0}, {"", 0}}, {{"EAX", LIM32}, {"", 0}, {"", 0}, {"", 0}}, "LAHF"},
    {OP_SAHF, &op_sahf, "sahf", {{"EAX", LIM32}, {"", 0}, {"", 0}, {"", 0}}, {{"", 0}, {"", 0}, {"", 0}, {"", 0}}, "SAHF"},
    {OP_SETC, &op_setc, "setc", {{"EAX", LIM32}, {"", 0}, {"", 0}, {"", 0}}, {{"EAX", LIM32}, {"", 0}, {"", 0}, {"", 0}}, "SETC AL"},
    {OP_SETNC, &op_setnc, "setnc", {{"EAX", LIM32}, {"", 0}, {"", 0}, {"", 0}}, {{"EAX", LIM32}, {"", 0}, {"", 0}, {"", 0}}, "SETNC AL"},
    {OP_SETZ, &op_setz, "setz", {{"EAX", LIM32}, {"", 0}, {"", 0}, {"", 0}}, {{"EAX", LIM32}, {"", 0}, {"", 0}, {"", 0}}, "SETZ AL"},
    {OP_SETNZ, &op_setnz, "setnz", {{"EAX", LIM32}, {"", 0}, {"", 0}, {"", 0}}, {{"EAX", LIM32}, {"", 0}, {"", 0}, {"", 0}}, "SETNZ AL"},
    {OP_SETS, &op_sets, "sets", {{"EAX", LIM32}, {"", 0}, {"", 0}, {"", 0}}, {{"EAX", LIM32}, {"", 0}, {"", 0}, {"", 0}}, "SETS AL"},
    {OP_SETNS, &op_setns, "setns", {{"EAX", LIM32}, {"", 0}, {"", 0}, {"", 0}}, {{"EAX", LIM32}, {"", 0}, {"", 0}, {"", 0}}, "SETNS AL"},
    {OP_SETO, &op_seto, "seto", {{"EAX", LIM32}, {"", 0}, {"", 0}, {"", 0}}, {{"EAX", LIM32}, {"", 0}, {"", 0}, {"", 0}}, "SETO AL"},
    {OP_SETNO, &op_setno, "setno", {{"EAX", LIM32}, {"", 0}, {"", 0}, {"", 0}}, {{"EAX", LIM32}, {"", 0}, {"", 0}, {"", 0}}, "SETNO AL"},
    {OP_SETP, &op_setp, "setp", {{"EAX", LIM32}, {"", 0}, {"", 0}, {"", 0}}, {{"EAX", LIM32}, {"", 0}, {"", 0}, {"", 0}}, "SETP AL"},
    {OP_SETNP, &op_setnp, "setnp", {{"EAX", LIM32}, {"", 0}, {"", 0}, {"", 0}}, {{"EAX", LIM32}, {"", 0}, {"", 0}, {"", 0}}, "SETNP AL"},
    {OP_CBW, &op_cbw, "cbw", {{"AX", LIM16}, {"", 0}, {"", 0}, {"", 0}}, {{"AX", LIM16}, {"", 0}, {"", 0}, {"", 0}}, "CBW"},
    {OP_CWDE, &op_cwde, "cwde", {{"AX", LIM16}, {"", 0}, {"", 0}, {"", 0}}, {{"EAX", LIM32}, {"", 0}, {"", 0}, {"", 0}}, "CWDE"},
    {OP_CWD, &op_cwd, "cwd", {{"AX", LIM16}, {"", 0}, {"", 0}, {"", 0}}, {{"AX", LIM16}, {"DX", LIM16}, {"", 0}, {"", 0}}, "CWD"},
    {OP_CDQ, &op_cdq, "cdq", {{"EAX", LIM32}, {"", 0}, {"", 0}, {"", 0}}, {{"EAX", LIM32}, {"EDX", LIM32}, {"", 0}, {"", 0}}, "CDQ"},
};
const OPCODE_RECORD asm_muldiv[] = {
    {OP_MUL, &op_mul, "mul", {{"EAX", LIM32}, {"ECX", LIM32}, {"", 0}, {"", 0}}, {{"EAX", LIM32}, {"EDX", LIM32}, {"", 0}, {"", 0}}, "MUL ECX"},
    {OP_IMUL, &op_imul, "imul", {{"EAX", LIM32}, {"ECX", LIM32}, {"", 0}, {"", 0}}, {{"EAX", LIM32}, {"EDX", LIM32}, {"", 0}, {"", 0}}, "IMUL ECX"},
    {OP_DIV, &op_div, "div", {{"EAX", LIM32}, {"ECX", LIM32}, {"EDX", LIM32}, {"", 0}}, {{"EAX", LIM32}, {"EDX", LIM32}, {"", 0}, {"", 0}}, "DIV ECX"},
    {OP_IDIV, &op_idiv, "idiv", {{"EAX", LIM32}, {"ECX", LIM32}, {"EDX", LIM32}, {"", 0}}, {{"EAX", LIM32}, {"EDX", LIM32}, {"", 0}, {"", 0}}, "IDIV ECX"},
};
const OPCODE_RECORD asm_shift[] = {
    {OP_SHR, &op_shr, "shr", {{"EAX", LIM32}, {"CL", LIMBS32}, {"", 0}, {"", 0}}, {{"EAX", LIM32}, {"", 0}, {"", 0}, {"", 0}}, "SHR EAX,CL"},
    {OP_SHL, &op_shl, "shl", {{"EAX", LIM32}, {"CL", LIMBS32}, {"", 0}, {"", 0}}, {{"EAX", LIM32}, {"", 0}, {"", 0}, {"", 0}}, "SHL EAX,CL"},
    {OP_SAR, &op_sar, "sar", {{"EAX", LIM32}, {"CL", LIMBS32}, {"", 0}, {"", 0}}, {{"EAX", LIM32}, {"", 0}, {"", 0}, {"", 0}}, "SAR EAX,CL"},
    {OP_SHLD, &op_shld, "shld", {{"EAX", LIM32}, {"EDX", LIM32}, {"CL", LIMBS32}, {"", 0}}, {{"EAX", LIM32}, {"", 0}, {"", 0}, {"", 0}}, "SHLD EAX,EDX,CL"},
    {OP_SHRD, &op_shrd, "shrd", {{"EAX", LIM32}, {"EDX", LIM32}, {"CL", LIMBS32}, {"", 0}}, {{"EAX", LIM32}, {"", 0}, {"", 0}, {"", 0}}, "SHRD EAX,EDX,CL"},
    {OP_ROL, &op_rol, "rol", {{"EAX", LIM32}, {"CL", LIMBS32}, {"", 0}, {"", 0}}, {{"EAX", LIM32}, {"", 0}, {"", 0}, {"", 0}}, "ROL EAX,CL"},
    {OP_ROR, &op_ror, "ror", {{"EAX", LIM32}, {"CL", LIMBS32}, {"", 0}, {"", 0}}, {{"EAX", LIM32}, {"", 0}, {"", 0}, {"", 0}}, "ROR EAX,CL"},
    {OP_RCL, &op_rcl, "rcl", {{"EAX", LIM32}, {"CL", LIMBS32}, {"", 0}, {"", 0}}, {{"EAX", LIM32}, {"", 0}, {"", 0}, {"", 0}}, "RCL EAX,CL"},
    {OP_RCR, &op_rcr, "rcr", {{"EAX", LIM32}, {"CL", LIMBS32}, {"", 0}, {"", 0}}, {{"EAX", LIM32}, {"", 0}, {"", 0}, {"", 0}}, "RCR EAX,CL"},
};
const OPCODE_RECORD asm_bits[] = {
    {OP_BT, &op_bt, "bt", {{"EAX", LIM32}, {"ECX", LIM32}, {"", 0}, {"", 0}}, {{"EAX", LIM32}, {"", 0}, {"", 0}, {"", 0}}, "BT EAX,ECX"},
    {OP_BTS, &op_bts, "bts", {{"EAX", LIM32}, {"ECX", LIM32}, {"", 0}, {"", 0}}, {{"EAX", LIM32}, {"", 0}, {"", 0}, {"", 0}}, "BTS EAX,ECX"},
    {OP_BTR, &op_btr, "btr", {{"EAX", LIM32}, {"ECX", LIM32}, {"", 0}, {"", 0}}, {{"EAX", LIM32}, {"", 0}, {"", 0}, {"", 0}}, "BTR EAX,ECX"},
    {OP_BTC, &op_btc, "btc", {{"EAX", LIM32}, {"ECX", LIM32}, {"", 0}, {"", 0}}, {{"EAX", LIM32}, {"", 0}, {"", 0}, {"", 0}}, "BTC EAX,ECX"},
    {OP_BSF, &op_bsf, "bsf", {{"EAX", LIM32}, {"ECX", LIM32}, {"", 0}, {"", 0}}, {{"EAX", LIM32}, {"", 0}, {"", 0}, {"", 0}}, "BSF EAX,ECX"},
    {OP_BSR, &op_bsr, "bsr", {{"EAX", LIM32}, {"ECX", LIM32}, {"", 0}, {"", 0}}, {{"EAX", LIM32}, {"", 0}, {"", 0}, {"", 0}}, "BSR EAX,ECX"},
};
const OPCODE_RECORD asm_bcd[] = {
    {OP_DAA, &op_daa, "daa", {{"AX", LIM16}, {"", 0}, {"", 0}, {"", 0}}, {{"AX", LIM16}, {"", 0}, {"", 0}, {"", 0}}, "DAA"},
    {OP_DAS, &op_das, "das", {{"AX", LIM16}, {"", 0}, {"", 0}, {"", 0}}, {{"AX", LIM16}, {"", 0}, {"", 0}, {"", 0}}, "DAS"},
    {OP_AAA, &op_aaa, "aaa", {{"AX", LIM16}, {"", 0}, {"", 0}, {"", 0}}, {{"AX", LIM16}, {"", 0}, {"", 0}, {"", 0}}, "AAA"},
    {OP_AAS, &op_aas, "aas", {{"AX", LIM16}, {"", 0}, {"", 0}, {"", 0}}, {{"AX", LIM16}, {"", 0}, {"", 0}, {"", 0}}, "AAS"},
    {OP_AAM, &op_aam, "aam", {{"AX", LIM16}, {"", 0}, {"", 0}, {"", 0}}, {{"AX", LIM16}, {"", 0}, {"", 0}, {"", 0}}, "AAM"},
    {OP_AAD, &op_aad, "aad", {{"AX", LIM16}, {"", 0}, {"", 0}, {"", 0}}, {{"AX", LIM16}, {"", 0}, {"", 0}, {"", 0}}, "AAD"},
};
const OPCODE_RECORD asm_special[] = {
    {OP_CLC, &op_clc, "clc", {{"", 0}, {"", 0}, {"", 0}, {"", 0}}, {{"", 0}, {"", 0}, {"", 0}, {"", 0}}, "CLC"},
    {OP_STC, &op_stc, "stc", {{"", 0}, {"", 0}, {"", 0}, {"", 0}}, {{"", 0}, {"", 0}, {"", 0}, {"", 0}}, "STC"},
    {OP_CMC, &op_cmc, "cmc", {{"", 0}, {"", 0}, {"", 0}, {"", 0}}, {{"", 0}, {"", 0}, {"", 0}, {"", 0}}, "CMC"},
    {OP_CMPXCHG, &op_cmpxchg, "cmpxchg", {{"EAX", LIM32}, {"ECX", LIM32}, {"EDX", LIM32}, {"", 0}}, {{"EAX", LIM32}, {"ECX", LIM32}, {"", 0}, {"", 0}}, "CMPXCHG ECX,EDX"},
    {OP_CPUID, &op_cpuid, "cpuid", {{"EAX", LIM32}, {"ECX", LIM32}, {"", 0}, {"", 0}}, {{"EAX", LIM32}, {"EDX", LIM32}, {"ECX", LIM32}, {"EBX", LIM32}}, "CPUID"},
    {OP_RDTSC, &op_rdtsc, "rdtsc", {{"", 0}, {"", 0}, {"", 0}, {"", 0}}, {{"EAX", LIM32}, {"EDX", LIM32}, {"", 0}, {"", 0}}, "RDTSC"},
};
#else
const OPCODE_RECORD asm_twooperands[] = {
    {OP_ADD, &op_add, "add", {{"RAX", LIM64}, {"RCX", LIM64}, {"", 0}, {"", 0}}, {{"RAX", LIM64}, {"", 0}, {"", 0}, {"", 0}}, "ADD RAX,RCX"},
    {OP_SUB, &op_sub, "sub", {{"RAX", LIM64}, {"RCX", LIM64}, {"", 0}, {"", 0}}, {{"RAX", LIM64}, {"", 0}, {"", 0}, {"", 0}}, "SUB RAX,RCX"},
    {OP_ADC, &op_adc, "adc", {{"RAX", LIM64}, {"RCX", LIM64}, {"", 0}, {"", 0}}, {{"RAX", LIM64}, {"", 0}, {"", 0}, {"", 0}}, "ADC RAX,RCX"},
    {OP_SBB, &op_sbb, "sbb", {{"RAX", LIM64}, {"RCX", LIM64}, {"", 0}, {"", 0}}, {{"RAX", LIM64}, {"", 0}, {"", 0}, {"", 0}}, "SBB RAX,RCX"},
    {OP_AND, &op_and, "and", {{"RAX", LIM64}, {"RCX", LIM64}, {"", 0}, {"", 0}}, {{"RAX", LIM64}, {"", 0}, {"", 0}, {"", 0}}, "AND RAX,RCX"},
    {OP_XOR, &op_xor, "xor", {{"RAX", LIM64}, {"RCX", LIM64}, {"", 0}, {"", 0}}, {{"RAX", LIM64}, {"", 0}, {"", 0}, {"", 0}}, "XOR RAX,RCX"},
    {OP_OR, &op_or, "or", {{"RAX", LIM64}, {"RCX", LIM64}, {"", 0}, {"", 0}}, {{"RAX", LIM64}, {"", 0}, {"", 0}, {"", 0}}, "OR RAX,RCX"},
    {OP_MOV, &op_mov, "mov", {{"RAX", LIM64}, {"RCX", LIM64}, {"", 0}, {"", 0}}, {{"RAX", LIM64}, {"", 0}, {"", 0}, {"", 0}}, "MOV RAX,RCX"},
    {OP_XCHG, &op_xchg, "xchg", {{"RAX", LIM64}, {"RCX", LIM64}, {"", 0}, {"", 0}}, {{"RAX", LIM64}, {"RCX", LIM64}, {"", 0}, {"", 0}}, "XCHG RAX,RCX"},
    {OP_TEST, &op_test, "test", {{"RAX", LIM64}, {"RCX", LIM64}, {"", 0}, {"", 0}}, {{"", 0}, {"", 0}, {"", 0}, {"", 0}}, "TEST RAX,RCX"},
    {OP_CMP, &op_cmp, "cmp", {{"RAX", LIM64}, {"RCX", LIM64}, {"", 0}, {"", 0}}, {{"", 0}, {"", 0}, {"", 0}, {"", 0}}, "CMP RAX,RCX"},
    {OP_XADD, &op_xadd, "xadd", {{"RAX", LIM64}, {"RCX", LIM64}, {"", 0}, {"", 0}}, {{"RAX", LIM64}, {"RCX", LIM64}, {"", 0}, {"", 0}}, "XADD RAX,RCX"},
    {OP_CMOVC, &op_cmovc, "cmovc", {{"RAX", LIM64}, {"RCX", LIM64}, {"", 0}, {"", 0}}, {{"RAX", LIM64}, {"", 0}, {"", 0}, {"", 0}}, "CMOVC RAX,RCX"},
    {OP_CMOVNC, &op_cmovnc, "cmovnc", {{"RAX", LIM64}, {"RCX", LIM64}, {"", 0}, {"", 0}}, {{"RAX", LIM64}, {"", 0}, {"", 0}, {"", 0}}, "CMOVNC RAX,RCX"},
    {OP_CMOVZ, &op_cmovz, "cmovz", {{"RAX", LIM64}, {"RCX", LIM64}, {"", 0}, {"", 0}}, {{"RAX", LIM64}, {"", 0}, {"", 0}, {"", 0}}, "CMOVZ RAX,RCX"},
    {OP_CMOVNZ, &op_cmovnz, "cmovnz", {{"RAX", LIM64}, {"RCX", LIM64}, {"", 0}, {"", 0}}, {{"RAX", LIM64}, {"", 0}, {"", 0}, {"", 0}}, "CMOVNZ RAX,RCX"},
    {OP_CMOVS, &op_cmovs, "cmovs", {{"RAX", LIM64}, {"RCX", LIM64}, {"", 0}, {"", 0}}, {{"RAX", LIM64}, {"", 0}, {"", 0}, {"", 0}}, "CMOVS RAX,RCX"},
    {OP_CMOVNS, &op_cmovns, "cmovns", {{"RAX", LIM64}, {"RCX", LIM64}, {"", 0}, {"", 0}}, {{"RAX", LIM64}, {"", 0}, {"", 0}, {"", 0}}, "CMOVNS RAX,RCX"},
    {OP_CMOVO, &op_cmovo, "cmovo", {{"RAX", LIM64}, {"RCX", LIM64}, {"", 0}, {"", 0}}, {{"RAX", LIM64}, {"", 0}, {"", 0}, {"", 0}}, "CMOVO RAX,RCX"},
    {OP_CMOVNO, &op_cmovno, "cmovno", {{"RAX", LIM64}, {"RCX", LIM64}, {"", 0}, {"", 0}}, {{"RAX", LIM64}, {"", 0}, {"", 0}, {"", 0}}, "CMOVNO RAX,RCX"},
    {OP_CMOVP, &op_cmovp, "cmovp", {{"RAX", LIM64}, {"RCX", LIM64}, {"", 0}, {"", 0}}, {{"RAX", LIM64}, {"", 0}, {"", 0}, {"", 0}}, "CMOVP RAX,RCX"},
    {OP_CMOVNP, &op_cmovnp, "cmovnp", {{"RAX", LIM64}, {"RCX", LIM64}, {"", 0}, {"", 0}}, {{"RAX", LIM64}, {"", 0}, {"", 0}, {"", 0}}, "CMOVNP RAX,RCX"},
    {OP_MOVZX_R8, &op_movzx_r8, "movzx r64,r8", {{"RAX", LIM64}, {"CL", LIM8}, {"", 0}, {"", 0}}, {{"RAX", LIM64}, {"", 0}, {"", 0}, {"", 0}}, "MOVZX RAX,CL"},
    {OP_MOVZX_R16, &op_movzx_r16, "movzx r64,r16", {{"RAX", LIM64}, {"CX", LIM16}, {"", 0}, {"", 0}}, {{"RAX", LIM64}, {"", 0}, {"", 0}, {"", 0}}, "MOVZX RAX,CX"},
    {OP_MOVSX_R8, &op_movsx_r8, "movsx r64,r8", {{"RAX", LIM64}, {"CL", LIM8}, {"", 0}, {"", 0}}, {{"RAX", LIM64}, {"", 0}, {"", 0}, {"", 0}}, "MOVSX RAX,CL"},
    {OP_MOVSX_R16, &op_movsx_r16, "movsx r64,r16", {{"RAX", LIM64}, {"CX", LIM16}, {"", 0}, {"", 0}}, {{"RAX", LIM64}, {"", 0}, {"", 0}, {"", 0}}, "MOVSX RAX,CX"},
    {OP_MOVSXD, &op_movsxd, "movsxd r64,r32", {{"RAX", LIM64}, {"ECX", LIM32}, {"", 0}, {"", 0}}, {{"RAX", LIM64}, {"", 0}, {"", 0}, {"", 0}}, "MOVSXD RAX,ECX"},
};
const OPCODE_RECORD asm_oneoperand[] = {
    {OP_INC, &op_inc, "inc", {{"RAX", LIM64}, {"", 0}, {"", 0}, {"", 0}}, {{"RAX", LIM64}, {"", 0}, {"", 0}, {"", 0}}, "INC RAX"},
    {OP_DEC, &op_dec, "dec", {{"RAX", LIM64}, {"", 0}, {"", 0}, {"", 0}}, {{"RAX", LIM64}, {"", 0}, {"", 0}, {"", 0}}, "DEC RAX"},
    {OP_NOT, &op_not, "not", {{"RAX", LIM64}, {"", 0}, {"", 0}, {"", 0}}, {{"RAX", LIM64}, {"", 0}, {"", 0}, {"", 0}}, "NOT RAX"},
    {OP_NEG, &op_neg, "neg", {{"RAX", LIM64}, {"", 0}, {"", 0}, {"", 0}}, {{"RAX", LIM64}, {"", 0}, {"", 0}, {"", 0}}, "NEG RAX"},
    {OP_BSWAP, &op_bswap, "bswap", {{"RAX", LIM64}, {"", 0}, {"", 0}, {"", 0}}, {{"RAX", LIM64}, {"", 0}, {"", 0}, {"", 0}}, "BSWAP RAX"},
    {OP_LAHF, &op_lahf, "lahf", {{"RAX", LIM64}, {"", 0}, {"", 0}, {"", 0}}, {{"RAX", LIM64}, {"", 0}, {"", 0}, {"", 0}}, "LAHF"},
    {OP_SAHF, &op_sahf, "sahf", {{"RAX", LIM64}, {"", 0}, {"", 0}, {"", 0}}, {{"", 0}, {"", 0}, {"", 0}, {"", 0}}, "SAHF"},
    {OP_SETC, &op_setc, "setc", {{"RAX", LIM64}, {"", 0}, {"", 0}, {"", 0}}, {{"RAX", LIM64}, {"", 0}, {"", 0}, {"", 0}}, "SETC AL"},
    {OP_SETNC, &op_setnc, "setnc", {{"RAX", LIM64}, {"", 0}, {"", 0}, {"", 0}}, {{"RAX", LIM64}, {"", 0}, {"", 0}, {"", 0}}, "SETNC AL"},
    {OP_SETZ, &op_setz, "setz", {{"RAX", LIM64}, {"", 0}, {"", 0}, {"", 0}}, {{"RAX", LIM64}, {"", 0}, {"", 0}, {"", 0}}, "SETZ AL"},
    {OP_SETNZ, &op_setnz, "setnz", {{"RAX", LIM64}, {"", 0}, {"", 0}, {"", 0}}, {{"RAX", LIM64}, {"", 0}, {"", 0}, {"", 0}}, "SETNZ AL"},
    {OP_SETS, &op_sets, "sets", {{"RAX", LIM64}, {"", 0}, {"", 0}, {"", 0}}, {{"RAX", LIM64}, {"", 0}, {"", 0}, {"", 0}}, "SETS AL"},
    {OP_SETNS, &op_setns, "setns", {{"RAX", LIM64}, {"", 0}, {"", 0}, {"", 0}}, {{"RAX", LIM64}, {"", 0}, {"", 0}, {"", 0}}, "SETNS AL"},
    {OP_SETO, &op_seto, "seto", {{"RAX", LIM64}, {"", 0}, {"", 0}, {"", 0}}, {{"RAX", LIM64}, {"", 0}, {"", 0}, {"", 0}}, "SETO AL"},
    {OP_SETNO, &op_setno, "setno", {{"RAX", LIM64}, {"", 0}, {"", 0}, {"", 0}}, {{"RAX", LIM64}, {"", 0}, {"", 0}, {"", 0}}, "SETNO AL"},
    {OP_SETP, &op_setp, "setp", {{"RAX", LIM64}, {"", 0}, {"", 0}, {"", 0}}, {{"RAX", LIM64}, {"", 0}, {"", 0}, {"", 0}}, "SETP AL"},
    {OP_SETNP, &op_setnp, "setnp", {{"RAX", LIM64}, {"", 0}, {"", 0}, {"", 0}}, {{"RAX", LIM64}, {"", 0}, {"", 0}, {"", 0}}, "SETNP AL"},
    {OP_CDQE, &op_cdqe, "cdqe", {{"EAX", LIM32}, {"", 0}, {"", 0}, {"", 0}}, {{"RAX", LIM64}, {"", 0}, {"", 0}, {"", 0}}, "CDQE"},
    {OP_CQO, &op_cqo, "cqo", {{"RAX", LIM64}, {"", 0}, {"", 0}, {"", 0}}, {{"RAX", LIM64}, {"RDX", LIM64}, {"", 0}, {"", 0}}, "CQO"},
};
const OPCODE_RECORD asm_muldiv[] = {
    {OP_MUL, &op_mul, "mul", {{"RAX", LIM64}, {"RCX", LIM64}, {"", 0}, {"", 0}}, {{"RAX", LIM64}, {"RDX", LIM64}, {"", 0}, {"", 0}}, "MUL RCX"},
    {OP_IMUL, &op_imul, "imul", {{"RAX", LIM64}, {"RCX", LIM64}, {"", 0}, {"", 0}}, {{"RAX", LIM64}, {"RDX", LIM64}, {"", 0}, {"", 0}}, "IMUL RCX"},
    {OP_DIV, &op_div, "div", {{"RAX", LIM64}, {"RCX", LIM64}, {"RDX", LIM64}, {"", 0}}, {{"RAX", LIM64}, {"RDX", LIM64}, {"", 0}, {"", 0}}, "DIV RCX"},
    {OP_IDIV, &op_idiv, "idiv", {{"RAX", LIM64}, {"RCX", LIM64}, {"RDX", LIM64}, {"", 0}}, {{"RAX", LIM64}, {"RDX", LIM64}, {"", 0}, {"", 0}}, "IDIV RCX"},
};
const OPCODE_RECORD asm_shift[] = {
    {OP_SHR, &op_shr, "shr", {{"RAX", LIM64}, {"CL", LIMBS64}, {"", 0}, {"", 0}}, {{"RAX", LIM64}, {"", 0}, {"", 0}, {"", 0}}, "SHR RAX,CL"},
    {OP_SHL, &op_shl, "shl", {{"RAX", LIM64}, {"CL", LIMBS64}, {"", 0}, {"", 0}}, {{"RAX", LIM64}, {"", 0}, {"", 0}, {"", 0}}, "SHL RAX,CL"},
    {OP_SAR, &op_sar, "sar", {{"RAX", LIM64}, {"CL", LIMBS64}, {"", 0}, {"", 0}}, {{"RAX", LIM64}, {"", 0}, {"", 0}, {"", 0}}, "SAR RAX,CL"},
    {OP_SHLD, &op_shld, "shld", {{"RAX", LIM64}, {"RDX", LIM64}, {"CL", LIMBS64}, {"", 0}}, {{"RAX", LIM64}, {"", 0}, {"", 0}, {"", 0}}, "SHLD RAX,RDX,CL"},
    {OP_SHRD, &op_shrd, "shrd", {{"RAX", LIM64}, {"RDX", LIM64}, {"CL", LIMBS64}, {"", 0}}, {{"RAX", LIM64}, {"", 0}, {"", 0}, {"", 0}}, "SHRD RAX,RDX,CL"},
    {OP_ROL, &op_rol, "rol", {{"RAX", LIM64}, {"CL", LIMBS64}, {"", 0}, {"", 0}}, {{"RAX", LIM64}, {"", 0}, {"", 0}, {"", 0}}, "ROL RAX,CL"},
    {OP_ROR, &op_ror, "ror", {{"RAX", LIM64}, {"CL", LIMBS64}, {"", 0}, {"", 0}}, {{"RAX", LIM64}, {"", 0}, {"", 0}, {"", 0}}, "ROR RAX,CL"},
    {OP_RCL, &op_rcl, "rcl", {{"RAX", LIM64}, {"CL", LIMBS64}, {"", 0}, {"", 0}}, {{"RAX", LIM64}, {"", 0}, {"", 0}, {"", 0}}, "RCL RAX,CL"},
    {OP_RCR, &op_rcr, "rcr", {{"RAX", LIM64}, {"CL", LIMBS64}, {"", 0}, {"", 0}}, {{"RAX", LIM64}, {"", 0}, {"", 0}, {"", 0}}, "RCR RAX,CL"},
};
const OPCODE_RECORD asm_bits[] = {
    {OP_BT, &op_bt, "bt", {{"RAX", LIM64}, {"RCX", LIM64}, {"", 0}, {"", 0}}, {{"RAX", LIM64}, {"", 0}, {"", 0}, {"", 0}}, "BT RAX,RCX"},
    {OP_BTS, &op_bts, "bts", {{"RAX", LIM64}, {"RCX", LIM64}, {"", 0}, {"", 0}}, {{"RAX", LIM64}, {"", 0}, {"", 0}, {"", 0}}, "BTS RAX,RCX"},
    {OP_BTR, &op_btr, "btr", {{"RAX", LIM64}, {"RCX", LIM64}, {"", 0}, {"", 0}}, {{"RAX", LIM64}, {"", 0}, {"", 0}, {"", 0}}, "BTR RAX,RCX"},
    {OP_BTC, &op_btc, "btc", {{"RAX", LIM64}, {"RCX", LIM64}, {"", 0}, {"", 0}}, {{"RAX", LIM64}, {"", 0}, {"", 0}, {"", 0}}, "BTC RAX,RCX"},
    {OP_BSF, &op_bsf, "bsf", {{"RAX", LIM64}, {"RCX", LIM64}, {"", 0}, {"", 0}}, {{"RAX", LIM64}, {"", 0}, {"", 0}, {"", 0}}, "BSF RAX,RCX"},
    {OP_BSR, &op_bsr, "bsr", {{"RAX", LIM64}, {"RCX", LIM64}, {"", 0}, {"", 0}}, {{"RAX", LIM64}, {"", 0}, {"", 0}, {"", 0}}, "BSR RAX,RCX"},
};
const OPCODE_RECORD asm_special[] = {
    {OP_CLC, &op_clc, "clc", {{"", 0}, {"", 0}, {"", 0}, {"", 0}}, {{"", 0}, {"", 0}, {"", 0}, {"", 0}}, "CLC"},
    {OP_STC, &op_stc, "stc", {{"", 0}, {"", 0}, {"", 0}, {"", 0}}, {{"", 0}, {"", 0}, {"", 0}, {"", 0}}, "STC"},
    {OP_CMC, &op_cmc, "cmc", {{"", 0}, {"", 0}, {"", 0}, {"", 0}}, {{"", 0}, {"", 0}, {"", 0}, {"", 0}}, "CMC"},
    {OP_CMPXCHG, &op_cmpxchg, "cmpxchg", {{"RAX", LIM64}, {"RCX", LIM64}, {"RDX", LIM64}, {"", 0}}, {{"RAX", LIM64}, {"RCX", LIM64}, {"", 0}, {"", 0}}, "CMPXCHG RCX,RDX"},
    {OP_CPUID, &op_cpuid, "cpuid", {{"EAX", LIM32}, {"ECX", LIM32}, {"", 0}, {"", 0}}, {{"EAX", LIM32}, {"EDX", LIM32}, {"ECX", LIM32}, {"EBX", LIM32}}, "CPUID"},
    {OP_RDTSC, &op_rdtsc, "rdtsc", {{"", 0}, {"", 0}, {"", 0}, {"", 0}}, {{"EAX", LIM32}, {"EDX", LIM32}, {"", 0}, {"", 0}}, "RDTSC"},
};
#endif
}  // namespace ASM_DEF

#endif  // ASM_DEFS_H
