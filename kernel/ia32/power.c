void power_halt(void) {
    __asm__ volatile (
        "cli\n"
        "hlt\n"
    );
}
