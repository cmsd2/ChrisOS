export GCC=$(TARGET)-gcc
export CC=$(TARGET)-gcc
export AS=$(TARGET)-as
export LD=$(TARGET)-ld
export AR=$(TARGET)-ar
export READELF=$(TARGET)-readelf
export CFLAGS:=$(CFLAGS) -W -Wall -Werror
