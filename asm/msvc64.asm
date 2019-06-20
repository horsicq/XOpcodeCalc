RECDATA64   STRUCT
OPERAND     DQ(2)       dup(?)
RESULT      DQ(2)       dup(?)
FLAG        DQ(2)       dup(?)
RECDATA64   ENDS

__ENTER     MACRO
            db          0F1h
            push        rbp
            mov         rbp,rsp
            push        rax
            push        rdx
            push        rcx
            push        rsi
            pushf
            
            mov         rsi,RCX ; // Microsoft x64 calling convention
            mov         rax,(RECDATA64 PTR [rsi]).OPERAND[0]
            mov         rcx,(RECDATA64 PTR [rsi]).OPERAND[8]
            mov         rdx,0
            push        (RECDATA64 PTR [rsi]).FLAG[0]
            popf
            ENDM
            
__LEAVE     MACRO
            mov         (RECDATA64 PTR [rsi]).RESULT[0],rax
            mov         (RECDATA64 PTR [rsi]).RESULT[8],rdx
            pushf       
            pop         (RECDATA64 PTR [rsi]).FLAG[8]
            
            popf
            pop         rsi
            pop         rcx
            pop         rdx
            pop         rax
            mov         rsp,rbp
            pop         rbp
            ret        
            ENDM

_TEXT       SEGMENT
op_add_64   PROC 
            __ENTER
            add         rax,rcx
            __LEAVE 
op_add_64   ENDP 

op_inc_64   PROC 
            __ENTER
            inc         rax
            __LEAVE 
op_inc_64   ENDP 
_TEXT       ENDS

END
