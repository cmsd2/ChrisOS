.section .text

// void stack_switch(struct context ** old, struct context * new)
.globl stack_switch
stack_switch:
        pushl %ebp
        movl %esp, %ebp

        pushl %ebx
        pushl %esi
        pushl %edi

_old_context:
        movl 8(%ebp), %eax
        test %eax, %eax
        jz _new_context
        movl %esp, (%eax)

_new_context:
        movl 12(%ebp), %eax
        movl %eax, %esp

        popl %edi
        popl %esi
        popl %ebx

        popl %ebp
        ret
