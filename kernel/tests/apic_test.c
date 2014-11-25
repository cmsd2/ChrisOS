#include <assert.h>
#include <tests/tests.h>
#include <arch/apic.h>
#include <sys/param.h>

int test_apic_macros(void) {
    assert(APIC_LVT_GET_VECTOR(-1) == 0xff);
    assert(APIC_LVT_GET_DELIVERY_STATUS(1 << 12) == 1);
    assert(APIC_LVT_GET_INPUT_PIN_POLARITY(1 << 13) == 1);
    assert(APIC_LVT_GET_REMOTE_IRR(1 << 14) == 1);
    assert(APIC_LVT_GET_TRIGGER_MODE(1 << 15) == 1);
    assert(APIC_LVT_GET_MASK(1 << 16) == 1);
    assert(APIC_LVT_GET_TIMER_MODE(3 << 17) == 3);
    assert(APIC_LVT_SET_VECTOR(0, -1) == 0xff);
    assert(APIC_LVT_SET_DELIVERY_STATUS(0, -1) == (1 << 12));
    assert(APIC_LVT_SET_INPUT_PIN_POLARITY(0, -1) == (1 << 13));
    assert(APIC_LVT_SET_REMOTE_IRR(0, -1) == (1 << 14));
    assert(APIC_LVT_SET_TRIGGER_MODE(0, -1) == (1 << 15));
    assert(APIC_LVT_SET_MASK(0, -1) == (1 << 16));
    assert(APIC_LVT_SET_TIMER_MODE(0, -1) == (3 << 17));

    assert(APIC_MSR_LOW_GET_BASE(0xfffff000) == 0xfffff);
    assert(APIC_MSR_LOW_GET_BASE(0x00000fff) == 0x0);
    assert(APIC_MSR_HIGH_GET_BASE(0xf) == 0xf00000);
    assert(APIC_MSR_HIGH_GET_BASE(0xfffffff0) == 0x0);
    assert(APIC_MSR_GET_BASE(0xf, 0xfffff000) == 0xffffff);
    assert(APIC_MSR_GET_BASE(0xfffffff0, 0x00000fff) == 0x0);
    assert(APIC_MSR_LOW_SET_BASE(0, 0xffffff) == 0xfffff000);
    assert(APIC_MSR_HIGH_SET_BASE(0, 0xffffff) == 0xf);
    
    return 0;
}
