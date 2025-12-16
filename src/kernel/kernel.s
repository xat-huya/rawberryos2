BITS 32
emain:
    mov byte [0xb8000], 'H'
    mov byte [0xb8001], 0x0F
    mov esp, 0x90000

    extern kmain
    call kmain
    jmp hang

hang:
    hlt
    jmp hang
