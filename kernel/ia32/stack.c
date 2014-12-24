#include <arch/stack.h>

struct context * stack_init(uintptr_t stack, size_t size,
                            stack_entry_point_func func, void * data) {

    // stack switch will return to our function pointer
    // arrange for stack to contain a stack frame
    // ready for stack_entry_point_func to start executing
    struct ep_frame {
        uint32_t eip;
        void * data;
    };

    struct context * ctx = (struct context *)stack
        + (size - sizeof(struct context) - sizeof(struct ep_frame));

    struct ep_frame args = {.eip = 0, .data = data};
    *(struct ep_frame*)ctx->frame.args = args;

    ctx->frame.eip = (uint32_t)func;
    ctx->frame.ebp = 0;

    return ctx;
}
