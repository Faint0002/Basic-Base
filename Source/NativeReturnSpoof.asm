.DATA
    addressToRet dq 0
    orginialRBX dq 0
.CODE
    ; Doing the modifying of our func address
    callAsm PROC
    	mov orginialRBX, rbx ; Moving RBX into our orginial RBX (dummy)
    	mov r9, [rsp] ; Moving R9 into RSP register
    	mov addressToRet, r9 ; Moving our address to return into R9
    	lea rbx, retAsm ; Lea'ing our rbx and retasm
    	mov [rsp], r8 ; We move the RSP register into R8
    	jmp rdx ; And then we jump to the RDX Register
    callAsm ENDP
    ; Restoring our RBX and og address then jumping
    retAsm PROC
    	mov rbx, orginialRBX ; We move our orginial RBX back into rbx, to restore it that is.
    	mov rcx, addressToRet ; Then we move our spoofed address into RCX
        jmp rcx ; Then we jump to RCX after modifying the address
    retAsm ENDP
END