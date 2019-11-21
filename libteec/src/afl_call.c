/*
 * AFL hypercalls
 */

#include "afl_call.h"

#include <stdlib.h>

static u_int64_t afl_call(u_int64_t a0, u_int64_t a1, u_int64_t a2) {
  u_int64_t ret;
  asm volatile(
      "str %[a2], [sp, #-16]!\n\t"
      "str %[a1], [sp, #-16]!\n\t"
      "str %[a0], [sp, #-16]!\n\t"
      "ldr x0, [sp], #16\n\t"
      "ldr x1, [sp], #16\n\t"
      "ldr x2, [sp], #16\n\t"
      "svc 0xfa32"
      : "=r"(ret)
      : [a0] "r"(a0), [a1] "r"(a1), [a2] "r"(a2));
  return ret;
}

int log_file(char* log_buffer, size_t size) {
  return afl_call(5, (u_int64_t)log_buffer, size);
}

