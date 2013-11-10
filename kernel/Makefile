
objs += main.o frame_alloc.o mem.o panic.o layout.o string.o terminal.o kprintf.o

CFLAGS += -I../include -I../arch/$(ARCH)/include

.PHONY: clean

kernel.a: $(objs)
	$(AR) -rs $@ $^

clean:
	$(RM) -f kernel.a $(objs)
