#ifndef SYS_IO_H
#define SYS_IO_H

unsigned char inb(unsigned short int port);
void outb(unsigned char value, unsigned short int port);
void io_wait();

#endif
