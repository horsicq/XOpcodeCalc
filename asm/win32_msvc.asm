.586
.MODEL FLAT, C
.STACK
.DATA
.CODE

RECDATA32   STRUCT
OPERAND     DD(2)       dup(?)
RESULT      DD(2)       dup(?)
FLAG        DD(2)       dup(?)
RECDATA32   ENDS

assume      esi:PTR RECDATA32

__ENTER     MACRO
            push        ebp
            mov         ebp,esp
            push        eax
            push        edx
            push        ecx
            push        esi
            pushf
            
            mov         esi,DWORD PTR[ebp+8]
            mov         eax,[esi].OPERAND[0]
            mov         ecx,[esi].OPERAND[4]
            mov         edx,0
            push        [esi].FLAG[0]
            popf
            ENDM
            
__LEAVE     MACRO
            mov         [esi].RESULT[0],eax
            mov         [esi].RESULT[4],edx
            pushf       
            pop         [esi].FLAG[4]
            
            popf
            pop         esi
            pop         ecx
            pop         edx
            pop         eax
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


END
