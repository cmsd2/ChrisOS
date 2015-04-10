.section .text

#define ISR_NOERRCODE(N) \
.globl isr ## N ; \
isr ## N : ; \
	push $ ## N ; \
	push $ ## N ; \
	jmp isr_initial_handler

#define ISR_ERRCODE(N) \
.globl isr ## N ; \
isr ## N : ; \
	push $ ## N ; \
	jmp isr_initial_handler

.globl isr_initial_handler
isr_initial_handler:
	pusha # push regs eax,ecx,edx,ebx,old esp,ebp,esi,edi

	mov %cr2, %eax
	pushl %eax # for page faults

	mov %ds, %eax
	push %eax

	mov $0x10, %ax # kernel data gdt entry
	mov %ax, %ds
	mov %ax, %es
	mov %ax, %fs
	mov %ax, %gs

	#TODO: what stack are we using here exactly?
isrs_initial_handler_handler_start:
	call interrupts_isr_handler
isrs_initial_handler_handler_done:
	pop %eax
	mov %ax, %ds
	mov %ax, %es
	mov %ax, %fs
	mov %ax, %gs

	add $4, %esp # unpush cr2
	popa

	add $8, %esp # unpush isr error code and irq number

isrs_initial_handler_iret:
	iret # rets and pops cs, ss, ip, sp and flags


ISR_NOERRCODE(0)
ISR_NOERRCODE(1)
ISR_NOERRCODE(2)
ISR_NOERRCODE(3)
ISR_NOERRCODE(4)
ISR_NOERRCODE(5)
ISR_NOERRCODE(6)
ISR_NOERRCODE(7)
ISR_ERRCODE(8)
ISR_NOERRCODE(9)
ISR_ERRCODE(10)
ISR_ERRCODE(11)
ISR_ERRCODE(12)
ISR_ERRCODE(13)
ISR_ERRCODE(14)
ISR_NOERRCODE(15)
ISR_NOERRCODE(16)
ISR_ERRCODE(17)
ISR_NOERRCODE(18)
ISR_NOERRCODE(19)
ISR_NOERRCODE(20)
ISR_NOERRCODE(21)
ISR_NOERRCODE(22)
ISR_NOERRCODE(23)
ISR_NOERRCODE(24)
ISR_NOERRCODE(25)
ISR_NOERRCODE(26)
ISR_NOERRCODE(27)
ISR_NOERRCODE(28)
ISR_NOERRCODE(29)
ISR_NOERRCODE(30)
ISR_NOERRCODE(31)

ISR_NOERRCODE(32)

// Spurious Interrupt Vector:
ISR_NOERRCODE(0x37)
        // IRQs mapped from here:
ISR_NOERRCODE(0x38)
ISR_NOERRCODE(0x39)
ISR_NOERRCODE(0x3a)
ISR_NOERRCODE(0x3b)
ISR_NOERRCODE(0x3c)
ISR_NOERRCODE(0x3d)
ISR_NOERRCODE(0x3e)
ISR_NOERRCODE(0x3f)
ISR_NOERRCODE(0x40)
ISR_NOERRCODE(0x41)
ISR_NOERRCODE(0x42)
ISR_NOERRCODE(0x43)
ISR_NOERRCODE(0x44)
ISR_NOERRCODE(0x45)
ISR_NOERRCODE(0x46)
ISR_NOERRCODE(0x47)
ISR_NOERRCODE(0x48)
ISR_NOERRCODE(0x49)
ISR_NOERRCODE(0x4a)
ISR_NOERRCODE(0x4b)
ISR_NOERRCODE(0x4c)
ISR_NOERRCODE(0x4d)
ISR_NOERRCODE(0x4e)
ISR_NOERRCODE(0x4f)

// system call
ISR_NOERRCODE(0x80)
