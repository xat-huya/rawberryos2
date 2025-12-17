BITS 32
global get_key
emain:
    mov byte [0xb8000], 'H'
    mov byte [0xb8001], 0x0F
    mov esp, 0x90000

    extern kmain
    call kmain
    jmp hang
get_key:
wait_loop:
    in al, 0x64        ; read keyboard status register
    test al, 1         ; bit 0 = output buffer full?
    jz wait_loop       ; loop until key pressed

    in al, 0x60        ; read scancode / ASCII from data port
    movzx eax, al  
    ret
hang:
    hlt
    jmp hang
