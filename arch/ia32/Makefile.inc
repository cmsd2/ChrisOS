export TARGET=i686-pc-elf
export CFLAGS:=$(CFLAGS) \
	-ffreestanding \
	-mno-mmx \
	-mno-sse \
	-mno-sse2 \
	-mno-sse3 \
	-mno-3dnow
