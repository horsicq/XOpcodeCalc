/* Copyright (c) 2019-2026 hors<horsicq@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#ifndef ASM_C_H
#define ASM_C_H

#include "asm.h"

// Portable C reference implementations of the opcode backends (ref_op_*).
//
// Contract (identical to the assembly backends):
//   input : A = OPERAND[0], C = OPERAND[1], D = OPERAND[2], B = OPERAND[3],
//           status flags loaded from FLAG[0] (only CF/PF/AF/ZF/SF/OF are honored)
//   output: RESULT[0] = A, RESULT[1] = D, RESULT[2] = C, RESULT[3] = B,
//           FLAG[1] = status flags | 0x202 (PUSHF image: reserved bit 1 + IF)
//
// Flags the architecture leaves undefined after an opcode are set to a fixed,
// documented choice here (see asm_c.cpp); tests must mask them out.
// ref_op_div / ref_op_idiv share the GUI precondition: the caller must have
// checked DivisionCheck::isDivisionDefined() first.
//
// On non-x86 targets asm_c.cpp also defines the op_* symbols themselves
// (XOPCODECALC_C_BACKEND), so the application runs without any assembly.

#define XOPCODECALC_OPS_COMMON(X) \
    X(add) X(sub) X(adc) X(sbb) X(and) X(xor) X(or) X(test) X(cmp) X(xadd) \
    X(movzx_r8) X(movzx_r16) X(movsx_r8) X(movsx_r16) \
    X(inc) X(dec) X(not) X(neg) X(bswap) \
    X(mul) X(imul) X(div) X(idiv) \
    X(shr) X(shl) X(sar) X(rol) X(ror) X(rcl) X(rcr) \
    X(cpuid) X(rdtsc) \
    X(bt) X(bsf) X(bsr) X(bts) X(btr) X(btc) \
    X(mov) X(xchg) X(cmpxchg) X(shld) X(shrd) \
    X(clc) X(stc) X(cmc) X(lahf) X(sahf) \
    X(setc) X(setnc) X(setz) X(setnz) X(sets) X(setns) X(seto) X(setno) X(setp) X(setnp) \
    X(cmovc) X(cmovnc) X(cmovz) X(cmovnz) X(cmovs) X(cmovns) X(cmovo) X(cmovno) X(cmovp) X(cmovnp) \
    X(popcnt) X(lzcnt) X(tzcnt) X(andn) X(bextr) X(blsi) X(blsmsk) X(blsr) X(bzhi) X(pdep) X(pext) \
    X(shlx) X(shrx) X(sarx) X(crc32) X(adcx) X(adox)

#ifdef OPCODE64
#define XOPCODECALC_OPS_WIDTH(X) X(movsxd) X(cdqe) X(cqo)
#else
#define XOPCODECALC_OPS_WIDTH(X) X(cbw) X(cwde) X(cwd) X(cdq) X(daa) X(das) X(aaa) X(aas) X(aam) X(aad)
#endif

#define XOPCODECALC_DECLARE_REF_OP(name) void ref_op_##name(RECDATA *pRecData);
XOPCODECALC_OPS_COMMON(XOPCODECALC_DECLARE_REF_OP)
XOPCODECALC_OPS_WIDTH(XOPCODECALC_DECLARE_REF_OP)
#undef XOPCODECALC_DECLARE_REF_OP

#endif  // ASM_C_H
