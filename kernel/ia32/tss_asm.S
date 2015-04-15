
// void tss_do_switch(void)
.globl tss_switch_to_user_mode
tss_switch_to_user_mode:    
    push %ebp
    mov %esp, %ebp
    
    cli

    call tss_set_stack_for_current_thread

    movw $0x23, %ax
    movw %ax, %ds
    movw %ax, %es
    movw %ax, %fs
    movw %ax, %gs

    mov %esp, %eax
    push $0x23 // user stack segment
    push %eax

    pushf
    pop %eax
    or $0x200, %eax
    push %eax // push flags with if bit set
    
    push $0x1b // user code segment

    push $1f

    iret
1:
    xor %eax, %eax

    pop %ebp
    
    ret

// void tss_flush(void)
.globl tss_flush
tss_flush:
    push %ebp
    mov %esp, %ebp

    mov $0x2b, %ax
    ltr %ax

    pop %ebp
    ret
