bits 16
org 0x7c00

xor ax, ax
mov ds, ax
mov es, ax
;

;
mov bx, 0x8000

mov ah, 0x02
mov al, 64
mov ch, 0
mov cl, 2
mov dh, 0
read_loop:
    int 0x13
    jc done          ; stop if error

    add bx, 512      ; next buffer
    inc cl           ; next sector
    jmp read_loop

done:

cli
lgdt [gdt_desc]

mov eax, cr0
or eax, 1
mov cr0, eax

jmp 0x08:pm_entry

gdt:
dq 0
dq 0x00cf9a000000ffff
dq 0x00cf92000000ffff

gdt_desc:
dw gdt_end - gdt - 1   ; size of GDT - 1
dd gdt                  ; linear address of GDT
gdt_end:

bits 32
pm_entry:
mov ax, 0x10
mov ds, ax
mov es, ax
mov ss, ax
mov esp, 0x80000
times 510-($-$$) db 0
dw 0xAA55
