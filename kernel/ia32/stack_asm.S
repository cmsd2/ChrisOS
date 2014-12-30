.section .text

// void stack_switch(struct context ** old, struct context * new)
.globl stack_switch
stack_switch:
        //convert the function call to a far-call stack layout:
        popl %eax // = ret eip
        pushfl
        pushl %cs
        pushl %eax
        // normal functions have args at 8(%ebp) and up
        // far-call functions have args at 16(%ebp) and up.

        pushl %ebp
        movl %esp, %ebp

        pushl %ebx
        pushl %esi
        pushl %edi

_old_context:
        movl 16(%ebp), %eax
        test %eax, %eax
        jz _new_context
        movl %esp, (%eax)

_new_context:
        movl 20(%ebp), %eax
        movl %eax, %esp

        popl %edi
        popl %esi
        popl %ebx

        popl %ebp

        iret
