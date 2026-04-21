RECDATA64   STRUCT
OPERAND     DQ(4)       dup(?)
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
            mov         rdx,(RECDATA64 PTR [rsi]).OPERAND[16]
            mov         rbx,(RECDATA64 PTR [rsi]).OPERAND[24]
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
;#################################
;# MOVZX R64,R8
;#################################
op_movzx_r8 PROC
            __ENTER
            movzx       rax,cl
            __LEAVE
op_movzx_r8 ENDP
;#################################
;# MOVZX R64,R16
;#################################
op_movzx_r16    PROC
            __ENTER
            movzx       rax,cx
            __LEAVE
op_movzx_r16    ENDP
;#################################
;# MOVSX R64,R8
;#################################
op_movsx_r8 PROC
            __ENTER
            movsx       rax,cl
            __LEAVE
op_movsx_r8 ENDP
;#################################
;# MOVSX R64,R16
;#################################
op_movsx_r16    PROC
            __ENTER
            movsx       rax,cx
            __LEAVE
op_movsx_r16    ENDP
;################################
; XADD
;################################
op_xadd     PROC
            __ENTER
            xadd        rax,rcx
            __LEAVE
op_xadd     ENDP
;################################
; MOV
;################################
op_mov      PROC
            __ENTER
            mov         rax,rcx
            __LEAVE
op_mov      ENDP
;################################
; XCHG
;################################
op_xchg     PROC
            __ENTER
            xchg        rax,rcx
            __LEAVE
op_xchg     ENDP
;################################
; CMPXCHG
;################################
op_cmpxchg  PROC
            __ENTER
            cmpxchg     rcx,rdx
            __LEAVE
op_cmpxchg  ENDP
;################################
; SHLD
;################################
op_shld     PROC
            __ENTER
            mov         rbx,rcx
            mov         cl,dl
            shld        rax,rbx,cl
            __LEAVE
op_shld     ENDP
;################################
; SHRD
;################################
op_shrd     PROC
            __ENTER
            mov         rbx,rcx
            mov         cl,dl
            shrd        rax,rbx,cl
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
            cmovc       rax,rcx
            __LEAVE
op_cmovc    ENDP
op_cmovnc   PROC
            __ENTER
            cmovnc      rax,rcx
            __LEAVE
op_cmovnc   ENDP
op_cmovz    PROC
            __ENTER
            cmovz       rax,rcx
            __LEAVE
op_cmovz    ENDP
op_cmovnz   PROC
            __ENTER
            cmovnz      rax,rcx
            __LEAVE
op_cmovnz   ENDP
op_cmovs    PROC
            __ENTER
            cmovs       rax,rcx
            __LEAVE
op_cmovs    ENDP
op_cmovns   PROC
            __ENTER
            cmovns      rax,rcx
            __LEAVE
op_cmovns   ENDP
op_cmovo    PROC
            __ENTER
            cmovo       rax,rcx
            __LEAVE
op_cmovo    ENDP
op_cmovno   PROC
            __ENTER
            cmovno      rax,rcx
            __LEAVE
op_cmovno   ENDP
op_cmovp    PROC
            __ENTER
            cmovp       rax,rcx
            __LEAVE
op_cmovp    ENDP
op_cmovnp   PROC
            __ENTER
            cmovnp      rax,rcx
            __LEAVE
op_cmovnp   ENDP
;################################
; MOVSXD
;################################
op_movsxd   PROC
            __ENTER
            movsxd      rax,ecx
            __LEAVE
op_movsxd   ENDP
;################################
; CDQE
;################################
op_cdqe     PROC
            __ENTER
            cdqe
            __LEAVE
op_cdqe     ENDP
;################################
; CQO
;################################
op_cqo      PROC
            __ENTER
            cqo
            __LEAVE
op_cqo      ENDP

_TEXT       ENDS

END
