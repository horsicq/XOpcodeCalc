.586
.MODEL FLAT, C
.STACK
.DATA
.CODE

RECDATA32   STRUCT
OPERAND     DD(2)       dup(?)
RESULT      DD(4)       dup(?)
FLAG        DD(2)       dup(?)
RECDATA32   ENDS

assume      esi:PTR RECDATA32

__ENTER     MACRO
            push        ebp
            mov         ebp,esp
            pusha
            pushf
            
            mov         esi,DWORD PTR[ebp+8]
            mov         eax,[esi].OPERAND[0]
            mov         ecx,[esi].OPERAND[4]
            mov         edx,0
            mov         ebx,0
            push        [esi].FLAG[0]
            popf
            ENDM
            
__LEAVE     MACRO
            mov         [esi].RESULT[0],eax
            mov         [esi].RESULT[4],edx
            mov         [esi].RESULT[8],ecx
            mov         [esi].RESULT[12],ebx
            pushf       
            pop         [esi].FLAG[4]
            
            popf
            popa
            mov         esp,ebp
            pop         ebp
            retn        
            ENDM
;################################
; ADD
;################################
op_add      PROC
            __ENTER
            add         eax,ecx
            __LEAVE 
op_add      ENDP
;################################
; SUB
;################################
op_sub     PROC
            __ENTER
            sub         eax,ecx
            __LEAVE
op_sub      ENDP
;################################
; ADC
;################################
op_adc     PROC
            __ENTER
            adc         eax,ecx
            __LEAVE
op_adc      ENDP
;################################
; SBB
;################################
op_sbb     PROC
            __ENTER
            sbb         eax,ecx
            __LEAVE
op_sbb      ENDP
;################################
; AND
;################################
op_and     PROC
            __ENTER
            and         eax,ecx
            __LEAVE
op_and      ENDP
;################################
; XOR
;################################
op_xor     PROC
            __ENTER
            xor         eax,ecx
            __LEAVE
op_xor      ENDP
;################################
; OR
;################################
op_or      PROC
            __ENTER
            or          eax,ecx
            __LEAVE
op_or       ENDP
;################################
; INC
;################################
op_inc      PROC 
            __ENTER
            inc         eax
            __LEAVE 
op_inc      ENDP 
;################################
; DEC
;################################
op_dec     PROC
            __ENTER
            dec         eax
            __LEAVE
op_dec      ENDP
;################################
; TEST
;################################
op_test     PROC
            __ENTER
            test        eax,ecx
            __LEAVE
op_test     ENDP
;################################
; CMP
;################################
op_cmp      PROC
            __ENTER
            cmp         eax,ecx
            __LEAVE
op_cmp      ENDP
;################################
; MUL
;################################
op_mul      PROC
            __ENTER
            mul         ecx
            __LEAVE
op_mul      ENDP
;################################
; IMUL
;################################
op_imul     PROC
            __ENTER
            imul        ecx
            __LEAVE
op_imul     ENDP
;################################
; DIV
;################################
op_div      PROC
            __ENTER
            div         ecx
            __LEAVE
op_div      ENDP
;################################
; IDIV
;################################
op_idiv     PROC
            __ENTER
            idiv        ecx
            __LEAVE
op_idiv     ENDP
;################################
; NOT
;################################
op_not     PROC
            __ENTER
            not         eax
            __LEAVE
op_not      ENDP
;################################
; NEG
;################################
op_neg     PROC
            __ENTER
            neg         eax
            __LEAVE
op_neg      ENDP
;################################
; SHR
;################################
op_shr     PROC
            __ENTER
            shr         eax,cl
            __LEAVE
op_shr      ENDP
;################################
; SHL
;################################
op_shl     PROC
            __ENTER
            shl         eax,cl
            __LEAVE
op_shl      ENDP
;################################
; SAR
;################################
op_sar     PROC
            __ENTER
            sar         eax,cl
            __LEAVE
op_sar      ENDP

;################################
; ROL
;################################
op_rol     PROC
            __ENTER
            rol         eax,cl
            __LEAVE
op_rol      ENDP
;################################
; ROR
;################################
op_ror     PROC
            __ENTER
            ror         eax,cl
            __LEAVE
op_ror      ENDP
;################################
; RCL
;################################
op_rcl     PROC
            __ENTER
            rcl         eax,cl
            __LEAVE
op_rcl      ENDP
;################################
; RCR
;################################
op_rcr     PROC
            __ENTER
            rcr         eax,cl
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
            bswap       eax
            __LEAVE
op_bswap    ENDP
;################################
; BSF
;################################
op_bsf     PROC
            __ENTER
            bsf         eax,ecx
            __LEAVE
op_bsf      ENDP
;################################
; BSR
;################################
op_bsr     PROC
            __ENTER
            bsr         eax,ecx
            __LEAVE
op_bsr      ENDP
;################################
; BT
;################################
op_bt      PROC
            __ENTER
            bt          eax,ecx
            __LEAVE
op_bt       ENDP
;################################
; BTS
;################################
op_bts     PROC
            __ENTER
            bts         eax,ecx
            __LEAVE
op_bts      ENDP
;################################
; BTR
;################################
op_btr     PROC
            __ENTER
            btr         eax,ecx
            __LEAVE
op_btr      ENDP
;################################
; BTC
;################################
op_btc     PROC
            __ENTER
            btc         eax,ecx
            __LEAVE
op_btc      ENDP
;################################
;################################
END
