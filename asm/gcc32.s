.intel_syntax noprefix
#ifdef __linux__
.section .note.GNU-stack,"",%progbits
#endif
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
#################################
# RDTSC
#################################
            .globl	op_rdtsc
op_rdtsc:
            __ENTER
            rdtsc
            __LEAVE
#################################
# BSWAP
#################################
            .globl	op_bswap
op_bswap:
            __ENTER
            bswap       eax
            __LEAVE
#################################
# BSF
#################################
            .globl	op_bsf
op_bsf:
            __ENTER
            bsf         eax,ecx
            __LEAVE
#################################
# BSR
#################################
            .globl	op_bsr
op_bsr:
            __ENTER
            bsr         eax,ecx
            __LEAVE
#################################
# BT
#################################
            .globl	op_bt
op_bt:
            __ENTER
            bt          eax,ecx
            __LEAVE
#################################
# BTS
#################################
            .globl	op_bts
op_bts:
            __ENTER
            bts         eax,ecx
            __LEAVE
#################################
# BTR
#################################
            .globl	op_btr
op_btr:
            __ENTER
            btr         eax,ecx
            __LEAVE
#################################
# BTC
#################################
            .globl	op_btc
op_btc:
            __ENTER
            btc         eax,ecx
            __LEAVE
#################################
# MOVZX R32,R8
#################################
            .globl	op_movzx_r8
op_movzx_r8:
            __ENTER
            movzx       eax,cl
            __LEAVE
#################################
# MOVZX R32,R16
#################################
            .globl	op_movzx_r16
op_movzx_r16:
            __ENTER
            movzx       eax,cx
            __LEAVE
#################################
# MOVSX R32,R8
#################################
            .globl	op_movsx_r8
op_movsx_r8:
            __ENTER
            movsx       eax,cl
            __LEAVE
#################################
# MOVSX R32,R16
#################################
            .globl	op_movsx_r16
op_movsx_r16:
            __ENTER
            movsx       eax,cx
            __LEAVE
#################################
# XADD
#################################
            .globl	op_xadd
op_xadd:
            __ENTER
            xadd        eax,ecx
            __LEAVE
#################################
# DAA
#################################
            .globl	op_daa
op_daa:
            __ENTER
            daa
            __LEAVE
#################################
# DAS
#################################
            .globl	op_das
op_das:
            __ENTER
            das
            __LEAVE
#################################
# AAA
#################################
            .globl	op_aaa
op_aaa:
            __ENTER
            aaa
            __LEAVE
#################################
# AAS
#################################
            .globl	op_aas
op_aas:
            __ENTER
            aas
            __LEAVE
#################################
# AAM
#################################
            .globl	op_aam
op_aam:
            __ENTER
            aam
            __LEAVE
#################################
# AAD
#################################
            .globl	op_aad
op_aad:
            __ENTER
            aad
            __LEAVE
