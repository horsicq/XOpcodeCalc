#include "../global.h"

#ifndef ASM_H
#define ASM_H
#ifdef OPCODE32
struct RECDATA32
{
    unsigned int OPERAND[2];
    unsigned int RESULT[2];
    unsigned int FLAG[2];
};
extern "C" void op_add_32(RECDATA32 *pRecData);
extern "C" void op_inc_32(RECDATA32 *pRecData);
#else
struct RECDATA64
{
    unsigned long long OPERAND[2];
    unsigned long long RESULT[2];
    unsigned long long FLAG[2];
};
extern "C" void op_add_64(RECDATA64 *pRecData) asm("op_add_64");
extern "C" void op_inc_64(RECDATA64 *pRecData);
#endif
#endif // ASM_H
