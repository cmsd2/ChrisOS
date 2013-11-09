
objs += main.o

.PHONY: clean

kernel.a: $(objs)
	$(AR) -rs $@ $^

clean:
	$(RM) -f kernel.a $(objs)
