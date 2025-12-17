typedef unsigned char  uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int   uint32_t;

#define VGA_ADDR 0xB8000
#define WHITE_ON_BLACK 0x0F
#define VGA_WIDTH 80
#define VGA_HEIGHT 25

volatile uint16_t* vga = (volatile uint16_t*)VGA_ADDR;
extern void exec_rba();
extern uint8_t get_key();
int VGA_POS;

unsigned char check_primary_ata(void) {
    unsigned char status;
    asm volatile (
        "inb %%dx, %%al"
        : "=a"(status)
        : "d"(0x1F7)
    );
    return status != 0xFF;
}

extern uint32_t exec(uint32_t addr);

uint32_t test_rba(uint32_t addr) {
    return exec(addr);
}

void print_newline() {
    int row = VGA_POS / VGA_WIDTH;
    row++;
    if (row >= VGA_HEIGHT) row = 0;  // wrap to top
    VGA_POS = row * VGA_WIDTH;
}

void clear_screen(void) {
    for (int i = 0; i < VGA_WIDTH * VGA_HEIGHT; i++) {
        vga[i] = ((uint16_t)WHITE_ON_BLACK << 8) | ' ';
    }
    VGA_POS = 0;
}

void print_chr(char c){
    vga[VGA_POS] = ((uint16_t)WHITE_ON_BLACK << 8) | c;
    VGA_POS++;
    if (VGA_POS >= VGA_WIDTH * VGA_HEIGHT) VGA_POS = 0;
}

void print_str(const char msg[]){
    for (int j = 0; msg[j]; j++){
        if (msg[j] == '\n'){
            print_newline();
        } else {
            print_chr(msg[j]);
        }
    }
}

void kernel_death(){
    const char msg[] = "KERNEL DEATH!\n";
    print_str(msg);
    while(1){}
}
char scancode_to_ascii(uint8_t sc) {
    // Top row: Q-P
        if (sc >= 0x02 && sc <= 0x0B) return '1' + (sc - 0x02);

    if (sc >= 0x10 && sc <= 0x19) {
        char map[] = {'q','w','e','r','t','y','u','i','o','p'};
        return map[sc - 0x10];
    }

    // Home row: A-L
    if (sc >= 0x1E && sc <= 0x26) {
        char map[] = {'a','s','d','f','g','h','j','k','l'};
        return map[sc - 0x1E];
    }

    // Bottom row: Z-M
    if (sc >= 0x2C && sc <= 0x32) {
        char map[] = {'z','x','c','v','b','n','m'};
        return map[sc - 0x2C];
    }

    return 0; // unknown / ignore
}


__attribute__((noreturn))
void kmain(void) {
    clear_screen();

    const char msg[] = "Hello, Rawberry!\n";
    const char ata[] = "ATA: Success.\n";
    const char nta[] = "ATA: Error.\n";

    print_str(msg);

    if (check_primary_ata()) {
        print_str(ata);
    } else {
        print_str(nta);
        kernel_death();
    }
    uint32_t result = exec(0x9000);   // eax return
    uint32_t ecx_val;                 // will hold edx

    asm volatile(
        "mov %%ecx, %0"
        : "=r"(ecx_val)   // output
    );

    char hex[] = "0123456789ABCDEF";

    // print eax low byte
    uint8_t j = result & 0xFF;
    print_chr(hex[(j >> 4) & 0xF]);
    print_chr(hex[j & 0xF]);

    print_chr(' ');

    // print ecx low byte
    uint8_t k = ecx_val & 0xFF;
    print_chr(hex[(k >> 4) & 0xF]);
    print_chr(hex[k & 0xF]);
    print_newline();
    int state;
    int shift = 0; // 0 = no shift, 1 = shift pressed

while(1) {
    uint8_t sc = get_key();

    // skip invalid
    if (!sc) continue;

    // Shift press/release
    if (sc == 0x2A || sc == 0x36) { shift = 1; continue; } // pressed
    if (sc == 0xAA || sc == 0xB6) { shift = 0; continue; } // released

    // ignore key releases for other keys
    if (sc & 0x80) continue;

    char c = 0;
    if (sc == 0x39) c = ' ';          // space
        else if (sc == 0x0E) {             // Backspace
        if (VGA_POS > 0) VGA_POS--;    // move cursor back
        vga[VGA_POS] = ((uint16_t)WHITE_ON_BLACK << 8) | ' '; // erase char
        continue;
    }
    else if (sc == 0x1C) {             // Enter
        print_newline();
        continue;
    }
    else c = scancode_to_ascii(sc);   // letters

    if (c) {
        if (shift && c >= 'a' && c <= 'z') c -= 32;
        print_chr(c);
    }
}



}
