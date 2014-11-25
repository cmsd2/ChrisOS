#include <assert.h>
#include <tests/tests.h>
#include <sys/param.h>

int test_bits(void) {
    assert(getbit(0xf0, 4) == 1);
    assert(getbit(0xf0, 3) == 0);
    assert(getbits(0xf0, 4, 7) == 0xf);
    assert(getbits(0xffffff0f, 4, 7) == 0x0);
    assert(withbits(0x0, 4, 7, 0xf) == 0xf0);
    assert(withbits(-1, 4, 7, 0x0) == 0xffffff0f);
    assert(bitmask(0, 0) == 1);
    assert(bitmask(0, 1) == 3);
    assert(bitmask(1, 1) == 2);
    assert(bitmask(1, 3) == 14);
    assert(bitmask(1, 31) == -2);
}
