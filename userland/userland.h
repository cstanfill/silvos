#ifndef __SILVOS_USERLAND_H
#define __SILVOS_USERLAND_H

#define CALLER_SAVE_REGISTERS "rax", "rcx", "rdx", "rsi", "rdi", "r8", "r9", "r10", "r11"
#define CALLER_SAVE_REGISTERS2 "rcx", "rdx", "rsi", "rdi", "r8", "r9", "r10", "r11"

static inline void yield (void) {
  __asm__ volatile("int $0x36" ::: CALLER_SAVE_REGISTERS);
}

static inline void putch (char c) {
  __asm__ volatile("int $0x37" : "=a" (c) : "a" (c) : CALLER_SAVE_REGISTERS2);
}

static inline void exit (void) {
  __asm__ volatile("int $0x38" ::: CALLER_SAVE_REGISTERS);
}

static inline char getch (void) {
  char c;
  __asm__ volatile("int $0x39" : "=a" (c) :: CALLER_SAVE_REGISTERS2);
  return c;
}

#endif
