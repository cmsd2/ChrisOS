libs = $(addprefix ../../, $(MODULES))
objs = boot.o paging.o gdt.o interrupts.o isrs.o idt.o cpuid.o apic.o msrs.o

INCLUDES = -I../../include -I./include
ASFLAGS += $(INCLUDES)
CFLAGS += $(INCLUDES)

.PHONY: all clean

all: kernel.bin

kernel.bin: $(objs) $(libs) link.ld
	$(CC) $(CFLAGS) -T link.ld -o $@ $^ -nostdlib -lgcc

clean:
	$(RM) -f kernel.bin *.o
