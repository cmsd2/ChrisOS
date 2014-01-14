#include <arch/io_ports.h>
#include <sys/io.h>

unsigned char inb(unsigned short int port) {
    unsigned char value;
    __asm__ volatile("inb %1, %0;" : "=&a"(value) : "r"(port));
    return value;
}

void outb(unsigned char value, unsigned short int port) {
    __asm__ volatile("outb %1, %0;" : : "r"(port), "a"(value));
}

