.section .text

#define ISR_NOERRCODE(N) \
.globl isr ## N ; \
isr ## N : ; \
	cli ; \
	push $ ## N ; \
	push $ ## N ; \
	jmp isr_initial_handler

#define ISR_ERRCODE(N) \
.globl isr ## N ; \
isr ## N : ; \
	cli ; \
	push $ ## N ; \
	jmp isr_initial_handler

#define IRQ(I, N) \
.globl irq ## I ; \
irq ## I : ; \
	cli ; \
	push $0 ; \
	push $ ## N ; \
	jmp irq_initial_handler

#define IOAPIC_IRQ(I, N) \
.globl ioapic_irq ## I          ; \
ioapic_irq ## I :               ; \
    cli                         ; \
    push $0                     ; \
    push $ ## N                 ; \
    jmp ioapic_irq_initial_handler

#define IOAPIC_IRQ_SIVR(N) \
.globl ioapic_irq_sivr      ; \
ioapic_irq_sivr :           ; \
    cli
    push $0
    push $0xff                  ; \
    jmp ioapic_irq_initial_handler
    
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
	call interrupts_isr_handler

	pop %eax
	mov %ax, %ds
	mov %ax, %es
	mov %ax, %fs
	mov %ax, %gs

	add $4, %esp # unpush cr2
	popa

	add $8, %esp # unpush isr error code and irq number

	iret # rets and pops cs, ss, ip, sp and flags

.globl irq_initial_handler
irq_initial_handler:
	pusha # push regs eax,ecx,edx,ebx,old esp,ebp,esi,edi

	mov %cr2, %eax
	pushl %eax

	mov %ds, %eax
	push %eax

	mov $0x10, %ax
	mov %ax, %ds
	mov %ax, %es
	mov %ax, %fs
	mov %ax, %gs

	call interrupts_irq_handler

	pop %eax
	mov %ax, %ds
	mov %ax, %es
	mov %ax, %fs
	mov %ax, %gs

	add $4, %esp # unpush cr2
	popa

	add $8, %esp # unpush isr error code and irq number

	iret # rets and pops cs, ss, ip, sp and flags
    
.globl ioapic_irq_initial_handler
ioapic_irq_initial_handler:
	pusha # push regs eax,ecx,edx,ebx,old esp,ebp,esi,edi

	mov %cr2, %eax
	pushl %eax

	mov %ds, %eax
	push %eax

	mov $0x10, %ax
	mov %ax, %ds
	mov %ax, %es
	mov %ax, %fs
	mov %ax, %gs

	call interrupts_ioapic_irq_handler

	pop %eax
	mov %ax, %ds
	mov %ax, %es
	mov %ax, %fs
	mov %ax, %gs

	add $4, %esp # unpush cr2
	popa

	add $8, %esp # unpush isr error code and irq number

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

// IRQs routed via PIC mapped from here:
IRQ ( 0, 32)
IRQ ( 1, 33)
IRQ ( 2, 34)
IRQ ( 3, 35)
IRQ ( 4, 36)
IRQ ( 5, 37)
IRQ ( 6, 38)
IRQ ( 7, 39)
IRQ ( 8, 40)
IRQ ( 9, 41)
IRQ (10, 42)
IRQ (11, 43)
IRQ (12, 44)
IRQ (13, 45)
IRQ (14, 46)
IRQ (15, 47)

// Spurious Interrupt Vector:
IOAPIC_IRQ_SIVR(55)
// IRQs routed via IOAPIC mapped from here:
IOAPIC_IRQ(0, 56)
IOAPIC_IRQ(1, 57)
IOAPIC_IRQ(2, 58)
IOAPIC_IRQ(3, 59)
IOAPIC_IRQ(4, 60)
IOAPIC_IRQ(5, 61)
IOAPIC_IRQ(6, 62)
IOAPIC_IRQ(7, 63)
IOAPIC_IRQ(8, 64)
IOAPIC_IRQ(9, 65)
IOAPIC_IRQ(10, 66)
IOAPIC_IRQ(11, 67)
IOAPIC_IRQ(12, 68)
IOAPIC_IRQ(13, 69)
IOAPIC_IRQ(14, 70)
IOAPIC_IRQ(15, 71)
IOAPIC_IRQ(16, 72)
IOAPIC_IRQ(17, 73)
IOAPIC_IRQ(18, 74)
IOAPIC_IRQ(19, 75)
IOAPIC_IRQ(20, 76)
IOAPIC_IRQ(21, 77)
IOAPIC_IRQ(22, 78)
IOAPIC_IRQ(23, 79)


// system call
ISR_NOERRCODE(0x80)
