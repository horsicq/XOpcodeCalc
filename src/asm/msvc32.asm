.686
.MODEL FLAT, C
.STACK
.DATA
.CODE

RECDATA32   STRUCT
OPERAND     DD(4)       dup(?)
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
            mov         edx,[esi].OPERAND[8]
            mov         ebx,[esi].OPERAND[12]
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
; RDTSC
;################################
op_rdtsc    PROC
            __ENTER
            rdtsc
            __LEAVE
op_rdtsc    ENDP
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
;#################################
;# MOVZX R32,R8
;#################################
op_movzx_r8 PROC
            __ENTER
            movzx       eax,cl
            __LEAVE
op_movzx_r8 ENDP
;#################################
;# MOVZX R32,R16
;#################################
op_movzx_r16    PROC
            __ENTER
            movzx       eax,cx
            __LEAVE
op_movzx_r16    ENDP
;#################################
;# MOVSX R32,R8
;#################################
op_movsx_r8 PROC
            __ENTER
            movsx       eax,cl
            __LEAVE
op_movsx_r8 ENDP
;#################################
;# MOVSX R32,R16
;#################################
op_movsx_r16    PROC
            __ENTER
            movsx       eax,cx
            __LEAVE
op_movsx_r16    ENDP
;################################
; XADD
;################################
op_xadd     PROC
            __ENTER
            xadd        eax,ecx
            __LEAVE
op_xadd     ENDP
;################################
; DAA
;################################
op_daa      PROC
            __ENTER
           daa
            __LEAVE
op_daa      ENDP
;################################
; DAS
;################################
op_das      PROC
            __ENTER
           das
            __LEAVE
op_das      ENDP
;################################
; AAA
;################################
op_aaa      PROC
            __ENTER
           aaa
            __LEAVE
op_aaa      ENDP
;################################
; AAS
;################################
op_aas      PROC
            __ENTER
           aas
            __LEAVE
op_aas      ENDP
;################################
; AAM
;################################
op_aam      PROC
            __ENTER
           aam
            __LEAVE
op_aam      ENDP
;################################
; AAD
;################################
op_aad      PROC
            __ENTER
           aad
            __LEAVE
op_aad      ENDP
;################################
; MOV
;################################
op_mov      PROC
            __ENTER
            mov         eax,ecx
            __LEAVE
op_mov      ENDP
;################################
; XCHG
;################################
op_xchg     PROC
            __ENTER
            xchg        eax,ecx
            __LEAVE
op_xchg     ENDP
;################################
; CMPXCHG
;################################
op_cmpxchg  PROC
            __ENTER
            cmpxchg     ecx,edx
            __LEAVE
op_cmpxchg  ENDP
;################################
; SHLD
;################################
op_shld     PROC
            __ENTER
            mov         ebx,ecx
            mov         cl,dl
            shld        eax,ebx,cl
            __LEAVE
op_shld     ENDP
;################################
; SHRD
;################################
op_shrd     PROC
            __ENTER
            mov         ebx,ecx
            mov         cl,dl
            shrd        eax,ebx,cl
            __LEAVE
op_shrd     ENDP
;################################
; CLC
;################################
op_clc      PROC
            __ENTER
            clc
            __LEAVE
op_clc      ENDP
;################################
; STC
;################################
op_stc      PROC
            __ENTER
            stc
            __LEAVE
op_stc      ENDP
;################################
; CMC
;################################
op_cmc      PROC
            __ENTER
            cmc
            __LEAVE
op_cmc      ENDP
;################################
; LAHF
;################################
op_lahf     PROC
            __ENTER
            lahf
            __LEAVE
op_lahf     ENDP
;################################
; SAHF
;################################
op_sahf     PROC
            __ENTER
            sahf
            __LEAVE
op_sahf     ENDP
;################################
; SETCC
;################################
op_setc     PROC
            __ENTER
            setc        al
            __LEAVE
op_setc     ENDP
op_setnc    PROC
            __ENTER
            setnc       al
            __LEAVE
op_setnc    ENDP
op_setz     PROC
            __ENTER
            setz        al
            __LEAVE
op_setz     ENDP
op_setnz    PROC
            __ENTER
            setnz       al
            __LEAVE
op_setnz    ENDP
op_sets     PROC
            __ENTER
            sets        al
            __LEAVE
op_sets     ENDP
op_setns    PROC
            __ENTER
            setns       al
            __LEAVE
op_setns    ENDP
op_seto     PROC
            __ENTER
            seto        al
            __LEAVE
op_seto     ENDP
op_setno    PROC
            __ENTER
            setno       al
            __LEAVE
op_setno    ENDP
op_setp     PROC
            __ENTER
            setp        al
            __LEAVE
op_setp     ENDP
op_setnp    PROC
            __ENTER
            setnp       al
            __LEAVE
op_setnp    ENDP
;################################
; CMOVCC
;################################
op_cmovc    PROC
            __ENTER
            cmovc       eax,ecx
            __LEAVE
op_cmovc    ENDP
op_cmovnc   PROC
            __ENTER
            cmovnc      eax,ecx
            __LEAVE
op_cmovnc   ENDP
op_cmovz    PROC
            __ENTER
            cmovz       eax,ecx
            __LEAVE
op_cmovz    ENDP
op_cmovnz   PROC
            __ENTER
            cmovnz      eax,ecx
            __LEAVE
op_cmovnz   ENDP
op_cmovs    PROC
            __ENTER
            cmovs       eax,ecx
            __LEAVE
op_cmovs    ENDP
op_cmovns   PROC
            __ENTER
            cmovns      eax,ecx
            __LEAVE
op_cmovns   ENDP
op_cmovo    PROC
            __ENTER
            cmovo       eax,ecx
            __LEAVE
op_cmovo    ENDP
op_cmovno   PROC
            __ENTER
            cmovno      eax,ecx
            __LEAVE
op_cmovno   ENDP
op_cmovp    PROC
            __ENTER
            cmovp       eax,ecx
            __LEAVE
op_cmovp    ENDP
op_cmovnp   PROC
            __ENTER
            cmovnp      eax,ecx
            __LEAVE
op_cmovnp   ENDP
;################################
; CBW
;################################
op_cbw      PROC
            __ENTER
            cbw
            __LEAVE
op_cbw      ENDP
;################################
; CWDE
;################################
op_cwde     PROC
            __ENTER
            cwde
            __LEAVE
op_cwde     ENDP
;################################
; CWD
;################################
op_cwd      PROC
            __ENTER
            cwd
            __LEAVE
op_cwd      ENDP
;################################
; CDQ
;################################
op_cdq      PROC
            __ENTER
            cdq
            __LEAVE
op_cdq      ENDP
;################################
;################################
END
