RECDATA64   STRUCT
OPERAND     DQ(2)       dup(?)
RESULT      DQ(4)       dup(?)
FLAG        DQ(2)       dup(?)
RECDATA64   ENDS

__ENTER     MACRO
            push        rbp
            mov         rbp,rsp
            push        rax
            push        rcx
            push        rdx
            push        rbx
            push        rsi
            push        rdi
            pushf
            
            mov         rsi,RCX ; // Microsoft x64 calling convention
            mov         rax,(RECDATA64 PTR [rsi]).OPERAND[0]
            mov         rcx,(RECDATA64 PTR [rsi]).OPERAND[8]
            mov         rdx,0
            mov         rbx,0
            push        (RECDATA64 PTR [rsi]).FLAG[0]
            popf
            ENDM
            
__LEAVE     MACRO
            mov         (RECDATA64 PTR [rsi]).RESULT[0],rax
            mov         (RECDATA64 PTR [rsi]).RESULT[8],rdx
            mov         (RECDATA64 PTR [rsi]).RESULT[16],rcx
            mov         (RECDATA64 PTR [rsi]).RESULT[24],rbx
            pushf       
            pop         (RECDATA64 PTR [rsi]).FLAG[8]
            
            popf
            pop         rdi
            pop         rsi
            pop         rbx
            pop         rdx
            pop         rcx
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
;################################
; TEST
;################################
op_test     PROC
            __ENTER
            test        rax,rcx
            __LEAVE
op_test     ENDP
;################################
; CMP
;################################
op_cmp      PROC
            __ENTER
            cmp         rax,rcx
            __LEAVE
op_cmp      ENDP
;################################
; MUL
;################################
op_mul      PROC
            __ENTER
            mul         rcx
            __LEAVE
op_mul      ENDP
;################################
; IMUL
;################################
op_imul     PROC
            __ENTER
            imul        rcx
            __LEAVE
op_imul     ENDP
;################################
; DIV
;################################
op_div      PROC
            __ENTER
            div         rcx
            __LEAVE
op_div      ENDP
;################################
; IDIV
;################################
op_idiv     PROC
            __ENTER
            idiv        rcx
            __LEAVE
op_idiv     ENDP
;################################
; NOT
;################################
op_not     PROC
            __ENTER
            not         rax
            __LEAVE
op_not      ENDP
;################################
; NEG
;################################
op_neg     PROC
            __ENTER
            neg         rax
            __LEAVE
op_neg      ENDP
;################################
; SHR
;################################
op_shr     PROC
            __ENTER
            shr         rax,cl
            __LEAVE
op_shr      ENDP
;################################
; SHL
;################################
op_shl     PROC
            __ENTER
            shl         rax,cl
            __LEAVE
op_shl      ENDP
;################################
; SAR
;################################
op_sar     PROC
            __ENTER
            sar         rax,cl
            __LEAVE
op_sar      ENDP
;################################
; ROL
;################################
op_rol     PROC
            __ENTER
            rol         rax,cl
            __LEAVE
op_rol      ENDP
;################################
; ROR
;################################
op_ror     PROC
            __ENTER
            ror         rax,cl
            __LEAVE
op_ror      ENDP
;################################
; RCL
;################################
op_rcl     PROC
            __ENTER
            rcl         rax,cl
            __LEAVE
op_rcl      ENDP
;################################
; RCR
;################################
op_rcr     PROC
            __ENTER
            rcr         rax,cl
            __LEAVE
op_rcr      ENDP
;################################
; CPUID
;################################
op_cpuid    PROC
            __ENTER
            cpuid
            __LEAVE
op_cpuid    ENDP
;################################
; BSWAP
;################################
op_bswap    PROC
            __ENTER
            bswap       rax
            __LEAVE
op_bswap    ENDP
;################################
; BSF
;################################
op_bsf      PROC
            __ENTER
            bsf         rax,rcx
            __LEAVE
op_bsf      ENDP
;################################
; BSR
;################################
op_bsr      PROC
            __ENTER
            bsr         rax,rcx
            __LEAVE
op_bsr      ENDP
;################################
; BT
;################################
op_bt       PROC
            __ENTER
            bt          rax,rcx
            __LEAVE
op_bt       ENDP
;################################
; BTS
;################################
op_bts      PROC
            __ENTER
            bts         rax,rcx
            __LEAVE
op_bts      ENDP
;################################
; BTR
;################################
op_btr      PROC
            __ENTER
            btr         rax,rcx
            __LEAVE
op_btr      ENDP
;################################
; BTC
;################################
op_btc      PROC
            __ENTER
            btc         rax,rcx
            __LEAVE
op_btc      ENDP
;################################
;################################
_TEXT       ENDS

END
