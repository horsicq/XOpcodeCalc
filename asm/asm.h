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
extern "C" void op_inc(RECDATA *pRecData) XASM("op_inc");
#endif // ASM_H
