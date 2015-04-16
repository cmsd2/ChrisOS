#ifndef SYSCALL_H
#define SYSCALL_H

#include <stdint.h>
#include <stdbool.h>
#include <arch/registers.h>

/*
 * from james molloy's syscall impl
 */

#define DECL_SYSCALL_0(NAME) long syscall_ ## NAME ()
#define DECL_SYSCALL_1(NAME, P1) long syscall_ ## NAME (P1)
#define DECL_SYSCALL_2(NAME, P1, P2) long syscall_ ## NAME (P1, P2)
#define DECL_SYSCALL_3(NAME, P1, P2, P3) long syscall_ ## NAME (P1, P2, P3)
#define DECL_SYSCALL_4(NAME, P1, P2, P3, P4) long syscall_ ## NAME (P1, P2, P3, P4)
#define DECL_SYSCALL_5(NAME, P1, P2, P3, P4, P5) long syscall_ ## NAME (P1, P2, P3, P4, P5)

#define DEFN_SYSCALL_0(NAME, NUM) \
long syscall_ ## NAME () \
{ \
  long result; \
  __asm__ volatile("int $0x80": "=a"(result) : "0" (NUM)); \
  return result; \
}

#define DEFN_SYSCALL_1(NAME, NUM, P1) \
long syscall_ ## NAME (P1 p1)	  \
{ \
  long result; \
  __asm__ volatile("int $0x80": "=a"(result) \
		   : "0" (NUM), \
                     "b" ((long)p1) \
  );	\
  return result; \
}

#define DEFN_SYSCALL_2(NAME, NUM, P1, P2) \
long syscall_ ## NAME (P1 p1, P2 p2)	  \
{ \
  long result; \
  __asm__ volatile("int $0x80": "=a"(result) \
		   : "0" (NUM), \
                     "b" ((long)p1) \
                     "c" ((long)p2) \
  );	\
  return result; \
}

#define DEFN_SYSCALL_3(NAME, NUM, P1, P2, P3) \
  long syscall_ ## NAME (P1 p1, P2 p2, P3 p3) \
{ \
  long result; \
  __asm__ volatile("int $0x80": "=a"(result) \
		   : "0" (NUM), \
                     "b" ((long)p1) \
                     "c" ((long)p2) \
                     "d" ((long)p3) \
  ); \
  return result; \
}
#define DEFN_SYSCALL_4(NAME, NUM, P1, P2, P3, P4) \
  long syscall_ ## NAME (P1 p1, P2 p2, P3 p3, P4 p4) \
{ \
  long result; \
  __asm__ volatile("int $0x80": "=a"(result) \
		   : "0" (NUM), \
                     "b" ((long)p1) \
                     "c" ((long)p2) \
                     "d" ((long)p3) \
                     "S" ((long)p4) \
  ); \
  return result; \
}

#define DEFN_SYSCALL_5(NAME, NUM, P1, P2, P3, P4, P5) \
  long syscall_ ## NAME (P1 p1, P2 p2, P3 p3, P4 p4, P5 p5) \
{ \
  long result; \
  __asm__ volatile("int $0x80": "=a"(result) \
		   : "0" (NUM), \
                     "b" ((long)p1) \
                     "c" ((long)p2) \
                     "d" ((long)p3) \
                     "S" ((long)p4) \
                     "D" ((long)p5) \
  ); \
  return result; \
}

DECL_SYSCALL_0(halt);

void syscalls_init(void);
bool syscalls_handler(uint32_t int_no, struct registers * regs, void * data);
void syscalls_call(uint32_t num, struct registers * regs);

#endif
