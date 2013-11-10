libs = $(addprefix ../../, $(MODULES))
objs = boot.o paging.o

INCLUDES = -I../../include -I./include
ASFLAGS += $(INCLUDES)
CFLAGS += $(INCLUDES)

.PHONY: all clean

all: kernel.bin

kernel.bin: $(objs) $(libs) link.ld
	$(CC) $(CFLAGS) -T link.ld -o $@ $^ -nostdlib -lgcc

clean:
	$(RM) -f kernel.bin *.o
