#include <arch/stack.h>

struct context * stack_init(uintptr_t stack, size_t size,
                            stack_entry_point_func func, void * data) {

    // stack switch will return to our function pointer
    // arrange for stack to contain a stack frame
    // ready for stack_entry_point_func to start executing
    // this is not the eip we return to in stack_switch.
    // it's the eip and args the c-calling convention thread
    // entry point function expects.
    // the context struct below is what contains the actual eip
    // plus cs and flags as required by the iret that stack_switch uses.
    struct ep_frame {
        uint32_t eip;
        void * data;
    };

    struct context * ctx = (struct context *)stack
        + (size - sizeof(struct context) - sizeof(struct ep_frame));

    struct ep_frame args = {.eip = 0, .data = data};
    *(struct ep_frame*)ctx->frame.args = args;

    ctx->frame.ebp = 0;
    ctx->frame.eip = (uint32_t)func;
    ctx->frame.cs = (uint32_t)get_cs_register();
    ctx->frame.eflags = flags_register_value_clear_bit(get_flags_register(), flags_if_bit);

    return ctx;
}
