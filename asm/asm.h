#include "../global.h"

#ifndef ASM_H
#define ASM_H
#ifdef OPCODE32
typedef unsigned int XVALUE;
#else
typedef unsigned long long XVALUE;
#endif
struct RECDATA
{
    XVALUE OPERAND[2];
    XVALUE RESULT[2];
    XVALUE FLAG[2];
};
#ifdef Q_OS_OSX
#define XASM(func) asm(func)
#else
#define XASM(func)
#endif

extern "C" void op_add(RECDATA *pRecData) XASM("op_add");
extern "C" void op_sub(RECDATA *pRecData) XASM("op_sub");
extern "C" void op_adc(RECDATA *pRecData) XASM("op_adc");
extern "C" void op_sbb(RECDATA *pRecData) XASM("op_sbb");
extern "C" void op_and(RECDATA *pRecData) XASM("op_and");
extern "C" void op_xor(RECDATA *pRecData) XASM("op_xor");
extern "C" void op_or(RECDATA *pRecData) XASM("op_or");
extern "C" void op_inc(RECDATA *pRecData) XASM("op_inc");
extern "C" void op_dec(RECDATA *pRecData) XASM("op_dec");
#endif // ASM_H
