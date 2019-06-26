.intel_syntax noprefix
.text

.macro      __ENTER
            push        rbp
            mov         rbp,rsp
            push        rax
            push        rdx
            push        rcx
            push        rsi
            pushf
            
            mov         rsi,rdi # System V
            mov         rax,QWORD PTR[rsi+0]
            mov         rcx,QWORD PTR[rsi+8]
            mov         rdx,0
            push        QWORD PTR[rsi+32]
            popf
.endm
            
.macro      __LEAVE
            mov         QWORD PTR[rsi+16],rax
            mov         QWORD PTR[rsi+24],rdx
            pushf       
            pop         QWORD PTR[rsi+40]
            
            popf
            pop         rsi
            pop         rcx
            pop         rdx
            pop         rax
            mov         rsp,rbp
            pop         rbp
            ret        
.endm
#################################
# ADD
#################################
            .globl	op_add
op_add:
            __ENTER
            add         rax,rcx
            __LEAVE
#################################
# SUB
#################################
            .globl	op_sub
op_sub:
            __ENTER
            sub         rax,rcx
            __LEAVE
#################################
# ADC
#################################
            .globl	op_adc
op_adc:
            __ENTER
            adc         rax,rcx
            __LEAVE
#################################
# SBB
#################################
            .globl	op_sbb
op_sbb:
            __ENTER
            sbb         rax,rcx
            __LEAVE
#################################
# AND
#################################
            .globl	op_and
op_and:
            __ENTER
            and         rax,rcx
            __LEAVE
#################################
# XOR
#################################
            .globl	op_xor
op_xor:
            __ENTER
            xor         rax,rcx
            __LEAVE
#################################
# OR
#################################
            .globl	op_or
op_or:
            __ENTER
            or          rax,rcx
            __LEAVE
#################################
# INC
#################################
            .globl	op_inc
op_inc:
            __ENTER
            inc         rax
            __LEAVE 
#################################
# DEC
#################################
            .globl	op_dec
op_dec:
            __ENTER
            dec         rax
            __LEAVE
