BITS 32
section .data
align 8
head:
    db 0, "RA", 32 ; signature
    db 2 ; exec
    dd _main ; body start
    db 0 ; privaldges, 0 lowest, 3 highest
    db 0 ; i dont know, it fix a bug, so im fine
_main: ; return 0
    mov eax, 0
    ret