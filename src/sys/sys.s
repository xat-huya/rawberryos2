BITS 32
global exec_rba
global exec
global state
extern VGA_POS

not_rba:
    mov eax, 1          ; return 1
    mov ecx, 8          ; error: non RBA program detected
    ret
not_execflag:
    mov eax, 1
    mov ecx, 7
    ret
non_zerointencional:
    mov ecx, 1
    ret
exec_rba:
    cmp byte [esi], 0
    jne not_rba
    cmp byte [esi+1], 'R'
    jne not_rba
    cmp byte [esi+2], 'A'
    jne not_rba
    cmp byte [esi+3], 32
    jne not_rba
    cmp byte [esi+4], 2
    jne not_execflag
    mov edx, [esi+6]    ; pointer to _main
    add edx, esi
    call edx
    cmp eax, 1
    je non_zerointencional
    ret
exec:
    push ebp
    mov ebp, esp

    push ebx
    push esi
    push edi

    xor eax, eax
    xor ebx, ebx
    xor ecx, ecx
    xor edx, edx
    xor edi, edi

    mov esi, [ebp + 8]
    call exec_rba

    pop edi
    pop esi
    pop ebx
    pop ebp
    ret
hang:
    hlt
    jmp hang