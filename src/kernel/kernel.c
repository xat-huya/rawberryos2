typedef unsigned char  uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int   uint32_t;

#define VGA_ADDR 0xB8000
#define WHITE_ON_BLACK 0x0F
#define VGA_WIDTH 80
#define VGA_HEIGHT 25

volatile uint16_t* vga = (volatile uint16_t*)VGA_ADDR;
extern void exec_rba();
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


    while(1) {}
}
