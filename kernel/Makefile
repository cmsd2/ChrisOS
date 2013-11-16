
objs += main.o frame_alloc.o mem.o panic.o layout.o string.o terminal.o kprintf.o multiboot.o

CFLAGS += -I../include -I../arch/$(ARCH)/include

.PHONY: clean

kernel.a: $(objs)
	$(AR) -rT $@ $^

clean:
	$(RM) -f kernel.a $(objs)
