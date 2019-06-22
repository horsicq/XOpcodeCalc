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
    
            .globl	_op_add_64
_op_add_64:
            __ENTER
            add         rax,rcx
            __LEAVE

            .globl	_op_inc_64
_op_inc_64:
            __ENTER
            inc         rax
            __LEAVE 
