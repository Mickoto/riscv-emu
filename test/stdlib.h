char * const stdout = (char *)0xb800;
char * const stdin = (char *)0xb804;
int * const stdexit = (int *)0xb808;

void main(void);

void putc(char c) {
    *stdout = c;
}

char getc() {
    return *stdin;
}

__attribute__((noreturn)) void exit(int code) {
    *stdexit = code;
    asm volatile ("ebreak");
    __builtin_unreachable();
}

void print(const char *str) {
    for (; *str; str++) {
        putc(*str);
    }
}

void __attribute__((noreturn, naked)) _start(void) {
    asm volatile (  "li sp, 0x1fffc\n"
                    "jal main\n"
                    "li a0, 0\n"
                    "jal exit\n");
}
