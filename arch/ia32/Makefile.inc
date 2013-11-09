export TARGET=i686-pc-elf
export CFLAGS=-ffreestanding \
	-std=c99 \
	-mno-mmx \
	-mno-sse \
	-mno-sse2 \
	-mno-sse3 \
	-mno-3dnow
