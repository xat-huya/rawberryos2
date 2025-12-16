#!/bin/sh
# in bash we trust
# that we are correct
# and not waste 2 hours
# per day - my dumbass every 7 hours
nasm -f bin src/boot/boot.s -o build/boot.bin
dd if=/dev/zero bs=1 count=$((512 - $(stat -c%s build/boot.bin))) >> build/boot.bin
nasm -f elf32 src/kernel/kernel.s -o build/kernel.o
nasm -f elf32 src/sys/sys.s -o build/sys.o
gcc -m32 -ffreestanding -fno-pic -fno-pie -c src/kernel/kernel.c -o build/kernel_c.o
ld -m elf_i386 -Ttext 0x8000 -o build/kernel.bin build/kernel.o build/sys.o build/kernel_c.o --oformat binary
nasm -f bin src/userspace/example.s -o user_build/example.rbe
cat build/boot.bin build/kernel.bin > rawberry.bin
RBA_MEM=0x9000 # where you want RBA to live in memory
KERNEL_LOAD=0x8000
BOOT_SIZE=512

# compute seek in file
SEEK=$((RBA_MEM - KERNEL_LOAD + BOOT_SIZE))

dd if=user_build/example.rbe of=rawberry.bin bs=1 seek=$SEEK conv=notrunc
