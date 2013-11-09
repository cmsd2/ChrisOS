libs = $(addprefix ../../, $(MODULES))

INCLUDES = -I../../include
ASFLAGS += $(INCLUDES)
CFLAGS += $(INCLUDES)

.PHONY: all clean

all: kernel.bin

kernel.bin: boot.o $(libs)
	$(CC) $(CFLAGS) -T link.ld -o $@ $^ -nostdlib -lgcc

clean:
	$(RM) -f kernel.bin *.o
