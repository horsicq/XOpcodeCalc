.intel_syntax noprefix
.text
    
.macro      __ENTER
            push        ebp
            mov         ebp,esp
            push        eax
            push        edx
            push        ecx
            push        esi
            pushf
            
            mov         esi,DWORD PTR[ebp+8]
            mov         eax,DWORD PTR[esi+0]
            mov         ecx,DWORD PTR[esi+4]
            mov         edx,0
            push        DWORD PTR[esi+24]
            popf
.endm
            
.macro      __LEAVE
            mov         DWORD PTR[esi+8],eax
            mov         DWORD PTR[esi+12],edx
            mov         DWORD PTR[esi+16],ecx
            mov         DWORD PTR[esi+20],ebx
            pushf       
            pop         DWORD PTR[esi+28]
            
            popf
            pop         esi
            pop         ecx
            pop         edx
            pop         eax
            mov         esp,ebp
            pop         ebp
            ret        
.endm
#################################
# ADD
#################################
            .globl	op_add
op_add:
            __ENTER
            add         eax,ecx
            __LEAVE
#################################
# SUB
#################################
            .globl	op_sub
op_sub:
            __ENTER
            sub         eax,ecx
            __LEAVE
#################################
# ADC
#################################
            .globl	op_adc
op_adc:
            __ENTER
            adc         eax,ecx
            __LEAVE
#################################
# SBB
#################################
            .globl	op_sbb
op_sbb:
            __ENTER
            sbb         eax,ecx
            __LEAVE
#################################
# AND
#################################
            .globl	op_and
op_and:
            __ENTER
            and         eax,ecx
            __LEAVE
#################################
# XOR
#################################
            .globl	op_xor
op_xor:
            __ENTER
            xor         eax,ecx
            __LEAVE
#################################
# OR
#################################
            .globl	op_or
op_or:
            __ENTER
            or          eax,ecx
            __LEAVE
#################################
# INC
#################################
            .globl	op_inc
op_inc:
            __ENTER
            inc         eax
            __LEAVE
#################################
# DEC
#################################
            .globl	op_dec
op_dec:
            __ENTER
            dec         eax
            __LEAVE
#################################
# TEST
#################################
            .globl	op_test
op_test:
            __ENTER
            test        eax,ecx
            __LEAVE
#################################
# CMP
#################################
            .globl	op_cmp
op_cmp:
            __ENTER
            cmp         eax,ecx
            __LEAVE
#################################
# MUL
#################################
            .globl	op_mul
op_mul:
            __ENTER
            mul         ecx
            __LEAVE
#################################
# IMUL
#################################
            .globl	op_imul
op_imul:
            __ENTER
            imul        ecx
            __LEAVE
#################################
# DIV
#################################
            .globl	op_div
op_div:
            __ENTER
            div         ecx
            __LEAVE
#################################
# IDIV
#################################
            .globl	op_idiv
op_idiv:
            __ENTER
            idiv        ecx
            __LEAVE
#################################
# NOT
#################################
            .globl	op_not
op_not:
            __ENTER
            not         eax
            __LEAVE
#################################
# NEG
#################################
            .globl	op_neg
op_neg:
            __ENTER
            neg         eax
            __LEAVE
#################################
# SHR
#################################
            .globl	op_shr
op_shr:
            __ENTER
            shr         eax,cl
            __LEAVE
#################################
# SHL
#################################
            .globl	op_shl
op_shl:
            __ENTER
            shl         eax,cl
            __LEAVE
#################################
# SAR
#################################
            .globl	op_sar
op_sar:
            __ENTER
            sar         eax,cl
            __LEAVE
#################################
# ROL
#################################
            .globl	op_rol
op_rol:
            __ENTER
            rol         eax,cl
            __LEAVE
#################################
# ROR
#################################
            .globl	op_ror
op_ror:
            __ENTER
            ror         eax,cl
            __LEAVE
#################################
# RCL
#################################
            .globl	op_rcl
op_rcl:
            __ENTER
            rcl         eax,cl
            __LEAVE
#################################
# RCR
#################################
            .globl	op_rcr
op_rcr:
            __ENTER
            rcr         eax,cl
            __LEAVE
#################################
# CPUID
#################################
            .globl	op_cpuid
op_cpuid:
            __ENTER
            cpuid
            __LEAVE
