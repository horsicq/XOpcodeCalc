.intel_syntax noprefix
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
    
	.globl	op_add_32
	.def	op_add_32;	.scl	2;	.type	32;	.endef
op_add_32:
            __ENTER
            add         eax,ecx
            __LEAVE 
    .globl	op_inc_32
	.def	op_inc_32;	.scl	2;	.type	32;	.endef
op_inc_32:
            __ENTER
            inc         eax
            __LEAVE 
