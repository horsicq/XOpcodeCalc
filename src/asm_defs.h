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
    OP_POPCNT,
    OP_LZCNT,
    OP_TZCNT,
    OP_ANDN,
    OP_BEXTR,
    OP_BLSI,
    OP_BLSMSK,
    OP_BLSR,
    OP_BZHI,
    OP_PDEP,
    OP_PEXT,
    OP_SHLX,
    OP_SHRX,
    OP_SARX,
    OP_CRC32,
    OP_ADCX,
    OP_ADOX,
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

// Shift/rotate counts are masked by the CPU to 0..31 / 0..63.
const unsigned char LIMBS32 = 0x1F;
const unsigned char LIMBS64 = 0x3F;
const unsigned char LIM8 = 0xFF;
const unsigned short LIM16 = 0xFFFF;
const unsigned int LIM32 = 0xFFFFFFFF;
const unsigned long long LIM64 = 0xFFFFFFFFFFFFFFFFull;

typedef void (*_asm_func)(RECDATA *pRecData);

// CPU feature an opcode needs; FEAT_BASE opcodes run everywhere. Rows that
// omit the field aggregate-initialize it to FEAT_BASE.
enum FEATURE {
    FEAT_BASE = 0,
    FEAT_POPCNT,
    FEAT_LZCNT,
    FEAT_BMI1,
    FEAT_BMI2,
    FEAT_SSE42,
    FEAT_ADX
};

struct OPCODE_RECORD {
    OP opcode;
    _asm_func asm_func;
    const char *pszName;
    VALUE_RECORD vrOperand[4];
    VALUE_RECORD vrResult[4];
    const char *pszExample;
    FEATURE feature;
};

// One table serves both widths: the register set is selected here and the row
// macros below splice it into names, limits and example strings.
#ifdef OPCODE32
#define XREG_A "EAX"
#define XREG_C "ECX"
#define XREG_D "EDX"
#define XREG_RN "r32"
#define XLIM LIM32
#define XLIMBS LIMBS32
#else
#define XREG_A "RAX"
#define XREG_C "RCX"
#define XREG_D "RDX"
#define XREG_RN "r64"
#define XLIM LIM64
#define XLIMBS LIMBS64
#endif

#define X_NONE \
    {          \
        "", 0  \
    }

// op A,C -> A                (add, cmov, bts, ...)
#define X_ROW_2OP(op, fn, name, EX) \
    {op, fn, name, {{XREG_A, XLIM}, {XREG_C, XLIM}, X_NONE, X_NONE}, {{XREG_A, XLIM}, X_NONE, X_NONE, X_NONE}, EX " " XREG_A "," XREG_C}
// op A,C -> A,C              (xchg, xadd)
#define X_ROW_2OP_BOTH(op, fn, name, EX) \
    {op, fn, name, {{XREG_A, XLIM}, {XREG_C, XLIM}, X_NONE, X_NONE}, {{XREG_A, XLIM}, {XREG_C, XLIM}, X_NONE, X_NONE}, EX " " XREG_A "," XREG_C}
// op A,C -> flags only       (test, cmp)
#define X_ROW_2OP_NORESULT(op, fn, name, EX) \
    {op, fn, name, {{XREG_A, XLIM}, {XREG_C, XLIM}, X_NONE, X_NONE}, {X_NONE, X_NONE, X_NONE, X_NONE}, EX " " XREG_A "," XREG_C}
// op A -> A                  (inc, not, bswap, ...)
#define X_ROW_1OP(op, fn, name, EX) \
    {op, fn, name, {{XREG_A, XLIM}, X_NONE, X_NONE, X_NONE}, {{XREG_A, XLIM}, X_NONE, X_NONE, X_NONE}, EX " " XREG_A}
// op with a fixed example    (lahf, setcc)
#define X_ROW_1OP_FIXED(op, fn, name, EXFULL) \
    {op, fn, name, {{XREG_A, XLIM}, X_NONE, X_NONE, X_NONE}, {{XREG_A, XLIM}, X_NONE, X_NONE, X_NONE}, EXFULL}
// mul/imul C -> A,D
#define X_ROW_MUL(op, fn, name, EX) \
    {op, fn, name, {{XREG_A, XLIM}, {XREG_C, XLIM}, X_NONE, X_NONE}, {{XREG_A, XLIM}, {XREG_D, XLIM}, X_NONE, X_NONE}, EX " " XREG_C}
// div/idiv C with D:A -> A,D
#define X_ROW_DIV(op, fn, name, EX) \
    {op, fn, name, {{XREG_A, XLIM}, {XREG_C, XLIM}, {XREG_D, XLIM}, X_NONE}, {{XREG_A, XLIM}, {XREG_D, XLIM}, X_NONE, X_NONE}, EX " " XREG_C}
// shift A,CL -> A
#define X_ROW_SHIFT(op, fn, name, EX) \
    {op, fn, name, {{XREG_A, XLIM}, {"CL", XLIMBS}, X_NONE, X_NONE}, {{XREG_A, XLIM}, X_NONE, X_NONE, X_NONE}, EX " " XREG_A ",CL"}
// shld/shrd A,D,CL -> A
#define X_ROW_SHIFTD(op, fn, name, EX) \
    {op, fn, name, {{XREG_A, XLIM}, {XREG_D, XLIM}, {"CL", XLIMBS}, X_NONE}, {{XREG_A, XLIM}, X_NONE, X_NONE, X_NONE}, EX " " XREG_A "," XREG_D ",CL"}
// op with no register operands (clc, stc, cmc)
#define X_ROW_FLAGSONLY(op, fn, name, EXFULL) \
    {op, fn, name, {X_NONE, X_NONE, X_NONE, X_NONE}, {X_NONE, X_NONE, X_NONE, X_NONE}, EXFULL}
// feature-gated op A,C -> A     (popcnt, blsi, crc32, ...)
#define X_ROW_2OP_F(op, fn, name, EX, feat) \
    {op, fn, name, {{XREG_A, XLIM}, {XREG_C, XLIM}, X_NONE, X_NONE}, {{XREG_A, XLIM}, X_NONE, X_NONE, X_NONE}, EX " " XREG_A "," XREG_C, feat}
// feature-gated op A,C,D -> A   (andn, bextr, shlx, ...); dlim bounds the D operand
#define X_ROW_3OP_F(op, fn, name, EX, dlim, feat)                                                                                        \
    {op, fn, name, {{XREG_A, XLIM}, {XREG_C, XLIM}, {XREG_D, dlim}, X_NONE}, {{XREG_A, XLIM}, X_NONE, X_NONE, X_NONE},                   \
     EX " " XREG_A "," XREG_C "," XREG_D, feat}

const OPCODE_RECORD asm_twooperands[] = {
    X_ROW_2OP(OP_ADD, &op_add, "add", "ADD"),
    X_ROW_2OP(OP_SUB, &op_sub, "sub", "SUB"),
    X_ROW_2OP(OP_ADC, &op_adc, "adc", "ADC"),
    X_ROW_2OP(OP_SBB, &op_sbb, "sbb", "SBB"),
    X_ROW_2OP(OP_AND, &op_and, "and", "AND"),
    X_ROW_2OP(OP_XOR, &op_xor, "xor", "XOR"),
    X_ROW_2OP(OP_OR, &op_or, "or", "OR"),
    X_ROW_2OP(OP_MOV, &op_mov, "mov", "MOV"),
    X_ROW_2OP_BOTH(OP_XCHG, &op_xchg, "xchg", "XCHG"),
    X_ROW_2OP_NORESULT(OP_TEST, &op_test, "test", "TEST"),
    X_ROW_2OP_NORESULT(OP_CMP, &op_cmp, "cmp", "CMP"),
    X_ROW_2OP_BOTH(OP_XADD, &op_xadd, "xadd", "XADD"),
    X_ROW_2OP_F(OP_ADCX, &op_adcx, "adcx", "ADCX", FEAT_ADX),
    X_ROW_2OP_F(OP_ADOX, &op_adox, "adox", "ADOX", FEAT_ADX),
    X_ROW_2OP(OP_CMOVC, &op_cmovc, "cmovc", "CMOVC"),
    X_ROW_2OP(OP_CMOVNC, &op_cmovnc, "cmovnc", "CMOVNC"),
    X_ROW_2OP(OP_CMOVZ, &op_cmovz, "cmovz", "CMOVZ"),
    X_ROW_2OP(OP_CMOVNZ, &op_cmovnz, "cmovnz", "CMOVNZ"),
    X_ROW_2OP(OP_CMOVS, &op_cmovs, "cmovs", "CMOVS"),
    X_ROW_2OP(OP_CMOVNS, &op_cmovns, "cmovns", "CMOVNS"),
    X_ROW_2OP(OP_CMOVO, &op_cmovo, "cmovo", "CMOVO"),
    X_ROW_2OP(OP_CMOVNO, &op_cmovno, "cmovno", "CMOVNO"),
    X_ROW_2OP(OP_CMOVP, &op_cmovp, "cmovp", "CMOVP"),
    X_ROW_2OP(OP_CMOVNP, &op_cmovnp, "cmovnp", "CMOVNP"),
    {OP_MOVZX_R8, &op_movzx_r8, "movzx " XREG_RN ",r8", {{XREG_A, XLIM}, {"CL", LIM8}, X_NONE, X_NONE}, {{XREG_A, XLIM}, X_NONE, X_NONE, X_NONE},
     "MOVZX " XREG_A ",CL"},
    {OP_MOVZX_R16, &op_movzx_r16, "movzx " XREG_RN ",r16", {{XREG_A, XLIM}, {"CX", LIM16}, X_NONE, X_NONE}, {{XREG_A, XLIM}, X_NONE, X_NONE, X_NONE},
     "MOVZX " XREG_A ",CX"},
    {OP_MOVSX_R8, &op_movsx_r8, "movsx " XREG_RN ",r8", {{XREG_A, XLIM}, {"CL", LIM8}, X_NONE, X_NONE}, {{XREG_A, XLIM}, X_NONE, X_NONE, X_NONE},
     "MOVSX " XREG_A ",CL"},
    {OP_MOVSX_R16, &op_movsx_r16, "movsx " XREG_RN ",r16", {{XREG_A, XLIM}, {"CX", LIM16}, X_NONE, X_NONE}, {{XREG_A, XLIM}, X_NONE, X_NONE, X_NONE},
     "MOVSX " XREG_A ",CX"},
#ifdef OPCODE64
    {OP_MOVSXD, &op_movsxd, "movsxd r64,r32", {{"RAX", LIM64}, {"ECX", LIM32}, X_NONE, X_NONE}, {{"RAX", LIM64}, X_NONE, X_NONE, X_NONE}, "MOVSXD RAX,ECX"},
#endif
};

const OPCODE_RECORD asm_oneoperand[] = {
    X_ROW_1OP(OP_INC, &op_inc, "inc", "INC"),
    X_ROW_1OP(OP_DEC, &op_dec, "dec", "DEC"),
    X_ROW_1OP(OP_NOT, &op_not, "not", "NOT"),
    X_ROW_1OP(OP_NEG, &op_neg, "neg", "NEG"),
    X_ROW_1OP(OP_BSWAP, &op_bswap, "bswap", "BSWAP"),
    X_ROW_1OP_FIXED(OP_LAHF, &op_lahf, "lahf", "LAHF"),
    {OP_SAHF, &op_sahf, "sahf", {{XREG_A, XLIM}, X_NONE, X_NONE, X_NONE}, {X_NONE, X_NONE, X_NONE, X_NONE}, "SAHF"},
    X_ROW_1OP_FIXED(OP_SETC, &op_setc, "setc", "SETC AL"),
    X_ROW_1OP_FIXED(OP_SETNC, &op_setnc, "setnc", "SETNC AL"),
    X_ROW_1OP_FIXED(OP_SETZ, &op_setz, "setz", "SETZ AL"),
    X_ROW_1OP_FIXED(OP_SETNZ, &op_setnz, "setnz", "SETNZ AL"),
    X_ROW_1OP_FIXED(OP_SETS, &op_sets, "sets", "SETS AL"),
    X_ROW_1OP_FIXED(OP_SETNS, &op_setns, "setns", "SETNS AL"),
    X_ROW_1OP_FIXED(OP_SETO, &op_seto, "seto", "SETO AL"),
    X_ROW_1OP_FIXED(OP_SETNO, &op_setno, "setno", "SETNO AL"),
    X_ROW_1OP_FIXED(OP_SETP, &op_setp, "setp", "SETP AL"),
    X_ROW_1OP_FIXED(OP_SETNP, &op_setnp, "setnp", "SETNP AL"),
#ifdef OPCODE64
    {OP_CDQE, &op_cdqe, "cdqe", {{"EAX", LIM32}, X_NONE, X_NONE, X_NONE}, {{"RAX", LIM64}, X_NONE, X_NONE, X_NONE}, "CDQE"},
    {OP_CQO, &op_cqo, "cqo", {{"RAX", LIM64}, X_NONE, X_NONE, X_NONE}, {{"RAX", LIM64}, {"RDX", LIM64}, X_NONE, X_NONE}, "CQO"},
#else
    {OP_CBW, &op_cbw, "cbw", {{"AX", LIM16}, X_NONE, X_NONE, X_NONE}, {{"AX", LIM16}, X_NONE, X_NONE, X_NONE}, "CBW"},
    {OP_CWDE, &op_cwde, "cwde", {{"AX", LIM16}, X_NONE, X_NONE, X_NONE}, {{"EAX", LIM32}, X_NONE, X_NONE, X_NONE}, "CWDE"},
    {OP_CWD, &op_cwd, "cwd", {{"AX", LIM16}, X_NONE, X_NONE, X_NONE}, {{"AX", LIM16}, {"DX", LIM16}, X_NONE, X_NONE}, "CWD"},
    {OP_CDQ, &op_cdq, "cdq", {{"EAX", LIM32}, X_NONE, X_NONE, X_NONE}, {{"EAX", LIM32}, {"EDX", LIM32}, X_NONE, X_NONE}, "CDQ"},
#endif
};

const OPCODE_RECORD asm_muldiv[] = {
    X_ROW_MUL(OP_MUL, &op_mul, "mul", "MUL"),
    X_ROW_MUL(OP_IMUL, &op_imul, "imul", "IMUL"),
    X_ROW_DIV(OP_DIV, &op_div, "div", "DIV"),
    X_ROW_DIV(OP_IDIV, &op_idiv, "idiv", "IDIV"),
};

const OPCODE_RECORD asm_shift[] = {
    X_ROW_SHIFT(OP_SHR, &op_shr, "shr", "SHR"),
    X_ROW_SHIFT(OP_SHL, &op_shl, "shl", "SHL"),
    X_ROW_SHIFT(OP_SAR, &op_sar, "sar", "SAR"),
    X_ROW_SHIFTD(OP_SHLD, &op_shld, "shld", "SHLD"),
    X_ROW_SHIFTD(OP_SHRD, &op_shrd, "shrd", "SHRD"),
    X_ROW_SHIFT(OP_ROL, &op_rol, "rol", "ROL"),
    X_ROW_SHIFT(OP_ROR, &op_ror, "ror", "ROR"),
    X_ROW_SHIFT(OP_RCL, &op_rcl, "rcl", "RCL"),
    X_ROW_SHIFT(OP_RCR, &op_rcr, "rcr", "RCR"),
    X_ROW_3OP_F(OP_SHLX, &op_shlx, "shlx", "SHLX", XLIMBS, FEAT_BMI2),
    X_ROW_3OP_F(OP_SHRX, &op_shrx, "shrx", "SHRX", XLIMBS, FEAT_BMI2),
    X_ROW_3OP_F(OP_SARX, &op_sarx, "sarx", "SARX", XLIMBS, FEAT_BMI2),
};

const OPCODE_RECORD asm_bits[] = {
    X_ROW_2OP(OP_BT, &op_bt, "bt", "BT"),
    X_ROW_2OP(OP_BTS, &op_bts, "bts", "BTS"),
    X_ROW_2OP(OP_BTR, &op_btr, "btr", "BTR"),
    X_ROW_2OP(OP_BTC, &op_btc, "btc", "BTC"),
    X_ROW_2OP(OP_BSF, &op_bsf, "bsf", "BSF"),
    X_ROW_2OP(OP_BSR, &op_bsr, "bsr", "BSR"),
    X_ROW_2OP_F(OP_POPCNT, &op_popcnt, "popcnt", "POPCNT", FEAT_POPCNT),
    X_ROW_2OP_F(OP_LZCNT, &op_lzcnt, "lzcnt", "LZCNT", FEAT_LZCNT),
    X_ROW_2OP_F(OP_TZCNT, &op_tzcnt, "tzcnt", "TZCNT", FEAT_BMI1),
    X_ROW_3OP_F(OP_ANDN, &op_andn, "andn", "ANDN", XLIM, FEAT_BMI1),
    X_ROW_3OP_F(OP_BEXTR, &op_bextr, "bextr", "BEXTR", LIM16, FEAT_BMI1),
    X_ROW_2OP_F(OP_BLSI, &op_blsi, "blsi", "BLSI", FEAT_BMI1),
    X_ROW_2OP_F(OP_BLSMSK, &op_blsmsk, "blsmsk", "BLSMSK", FEAT_BMI1),
    X_ROW_2OP_F(OP_BLSR, &op_blsr, "blsr", "BLSR", FEAT_BMI1),
    X_ROW_3OP_F(OP_BZHI, &op_bzhi, "bzhi", "BZHI", LIM8, FEAT_BMI2),
    X_ROW_3OP_F(OP_PDEP, &op_pdep, "pdep", "PDEP", XLIM, FEAT_BMI2),
    X_ROW_3OP_F(OP_PEXT, &op_pext, "pext", "PEXT", XLIM, FEAT_BMI2),
};

#ifndef OPCODE64
const OPCODE_RECORD asm_bcd[] = {
    {OP_DAA, &op_daa, "daa", {{"AX", LIM16}, X_NONE, X_NONE, X_NONE}, {{"AX", LIM16}, X_NONE, X_NONE, X_NONE}, "DAA"},
    {OP_DAS, &op_das, "das", {{"AX", LIM16}, X_NONE, X_NONE, X_NONE}, {{"AX", LIM16}, X_NONE, X_NONE, X_NONE}, "DAS"},
    {OP_AAA, &op_aaa, "aaa", {{"AX", LIM16}, X_NONE, X_NONE, X_NONE}, {{"AX", LIM16}, X_NONE, X_NONE, X_NONE}, "AAA"},
    {OP_AAS, &op_aas, "aas", {{"AX", LIM16}, X_NONE, X_NONE, X_NONE}, {{"AX", LIM16}, X_NONE, X_NONE, X_NONE}, "AAS"},
    {OP_AAM, &op_aam, "aam", {{"AX", LIM16}, X_NONE, X_NONE, X_NONE}, {{"AX", LIM16}, X_NONE, X_NONE, X_NONE}, "AAM"},
    {OP_AAD, &op_aad, "aad", {{"AX", LIM16}, X_NONE, X_NONE, X_NONE}, {{"AX", LIM16}, X_NONE, X_NONE, X_NONE}, "AAD"},
};
#endif

const OPCODE_RECORD asm_special[] = {
    X_ROW_FLAGSONLY(OP_CLC, &op_clc, "clc", "CLC"),
    X_ROW_FLAGSONLY(OP_STC, &op_stc, "stc", "STC"),
    X_ROW_FLAGSONLY(OP_CMC, &op_cmc, "cmc", "CMC"),
    {OP_CMPXCHG, &op_cmpxchg, "cmpxchg", {{XREG_A, XLIM}, {XREG_C, XLIM}, {XREG_D, XLIM}, X_NONE}, {{XREG_A, XLIM}, {XREG_C, XLIM}, X_NONE, X_NONE},
     "CMPXCHG " XREG_C "," XREG_D},
    {OP_CPUID, &op_cpuid, "cpuid", {{"EAX", LIM32}, {"ECX", LIM32}, X_NONE, X_NONE}, {{"EAX", LIM32}, {"EDX", LIM32}, {"ECX", LIM32}, {"EBX", LIM32}}, "CPUID"},
    {OP_RDTSC, &op_rdtsc, "rdtsc", {X_NONE, X_NONE, X_NONE, X_NONE}, {{"EAX", LIM32}, {"EDX", LIM32}, X_NONE, X_NONE}, "RDTSC"},
    X_ROW_2OP_F(OP_CRC32, &op_crc32, "crc32", "CRC32", FEAT_SSE42),
};

#undef XREG_A
#undef XREG_C
#undef XREG_D
#undef XREG_RN
#undef XLIM
#undef XLIMBS
#undef X_NONE
#undef X_ROW_2OP
#undef X_ROW_2OP_BOTH
#undef X_ROW_2OP_NORESULT
#undef X_ROW_1OP
#undef X_ROW_1OP_FIXED
#undef X_ROW_MUL
#undef X_ROW_DIV
#undef X_ROW_SHIFT
#undef X_ROW_SHIFTD
#undef X_ROW_FLAGSONLY
#undef X_ROW_2OP_F
#undef X_ROW_3OP_F
}  // namespace ASM_DEF

#endif  // ASM_DEFS_H
