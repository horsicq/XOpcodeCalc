RECDATA64   STRUCT
OPERAND     DQ(2)       dup(?)
RESULT      DQ(2)       dup(?)
FLAG        DQ(2)       dup(?)
RECDATA64   ENDS

__ENTER     MACRO
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
;################################
; ADD
;################################
op_add      PROC
            __ENTER
            add         rax,rcx
            __LEAVE 
op_add      ENDP
;################################
; SUB
;################################
op_sub      PROC
            __ENTER
            sub         rax,rcx
            __LEAVE
op_sub      ENDP
;################################
; ADC
;################################
op_adc      PROC
            __ENTER
            adc         rax,rcx
            __LEAVE
op_adc      ENDP
;################################
; SBB
;################################
op_sbb      PROC
            __ENTER
            sbb         rax,rcx
            __LEAVE
op_sbb      ENDP
;################################
; AND
;################################
op_and      PROC
            __ENTER
            and         rax,rcx
            __LEAVE
op_and      ENDP
;################################
; XOR
;################################
op_xor      PROC
            __ENTER
            xor         rax,rcx
            __LEAVE
op_xor      ENDP
;################################
; OR
;################################
op_or       PROC
            __ENTER
            or          rax,rcx
            __LEAVE
op_or       ENDP
;################################
; INC
;################################
op_inc      PROC
            __ENTER
            inc         rax
            __LEAVE 
op_inc      ENDP
;################################
; DEC
;################################
op_dec      PROC
            __ENTER
            dec         rax
            __LEAVE
op_dec      ENDP
_TEXT       ENDS

END
