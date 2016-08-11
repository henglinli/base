// -*-coding:utf-8-unix;-*-
#pragma once
// copy of https://github.com/halayli/lthread/blob/master/src/lthread.c
__asm__ (
         ".text\n"
         ".p2align 4,,15\n"
         ".globl _switch\n"
         "_switch:\n"
         "mov %rsp, 0(%rsi)      # save stack_pointer     \n"
         "mov %rbp, 8(%rsi)      # save frame_pointer     \n"
         "mov (%rsp), %rax       # save insn_pointer      \n"
         "mov %rax, 16(%rsi)                              \n"
         "mov %rbx, 24(%rsi)     # save rbx,r12-r15       \n"
         "mov %r12, 32(%rsi)                              \n"
         "mov %r13, 40(%rsi)                              \n"
         "mov %r14, 48(%rsi)                              \n"
         "mov %r15, 56(%rsi)                              \n"
         "mov 56(%rdi), %r15                              \n"
         "mov 48(%rdi), %r14                              \n"
         "mov 40(%rdi), %r13     # restore rbx,r12-r15    \n"
         "mov 32(%rdi), %r12                              \n"
         "mov 24(%rdi), %rbx                              \n"
         "mov 8(%rdi), %rbp      # restore frame_pointer  \n"
         "mov 0(%rdi), %rsp      # restore stack_pointer  \n"
         "mov 16(%rdi), %rax     # restore insn_pointer   \n"
         "mov %rax, (%rsp)                                \n"
         "ret                                             \n"
         );
  /*
   * CPU context registers
   */
struct ctx {
  void	*rsp;		/* 0  */
  void	*rbp;		/* 8  */
  void	*rip;		/* 16 */
  void	*rbx;		/* 24 */
  void	*r12;		/* 32 */
  void	*r13;		/* 40 */
  void	*r14;		/* 48 */
  void	*r15;		/* 56 */
};
  //
void _switch(ctx* to, ctx* from);
static inline void ctx_switch(ctx* to, ctx* from) {
  #if 0
  #if 0
  _switch(to, from);
  #else
  __asm__ (
           "mov %rsp, 0(%rsi)      # save stack_pointer     \n"
           "mov %rbp, 8(%rsi)      # save frame_pointer     \n"
           "mov (%rsp), %rax       # save insn_pointer      \n"
           "mov %rax, 16(%rsi)                              \n"
           "mov %rbx, 24(%rsi)     # save rbx,r12-r15       \n"
           "mov %r12, 32(%rsi)                              \n"
           "mov %r13, 40(%rsi)                              \n"
           "mov %r14, 48(%rsi)                              \n"
           "mov %r15, 56(%rsi)                              \n"
           "mov 56(%rdi), %r15                              \n"
           "mov 48(%rdi), %r14                              \n"
           "mov 40(%rdi), %r13     # restore rbx,r12-r15    \n"
           "mov 32(%rdi), %r12                              \n"
           "mov 24(%rdi), %rbx                              \n"
           "mov 8(%rdi), %rbp      # restore frame_pointer  \n"
           "mov 0(%rdi), %rsp      # restore stack_pointer  \n"
           "mov 16(%rdi), %rax     # restore insn_pointer   \n"
           "mov %rax, (%rsp)                                \n"
           "ret                                             \n"
           );
  #endif
  #else
  __asm__ (
           "mov %%rsp, %c[rsp](%1)      #save stack pointer    \n"
           "mov %%rbp, %c[rbp](%1)      #save frame pointer    \n"
           "mov (%%rsp), %%rax          #save insn pointer     \n"
           "mov %%rax, %c[rip](%1)                             \n"
           "mov %%rbx, %c[rbx](%1)      #save rbx              \n"
           "mov %%r12, %c[r12](%1)      #save r12-r15          \n"
           "mov %%r13, %c[r13](%1)                             \n"
           "mov %%r14, %c[r14](%1)                             \n"
           "mov %%r15, %c[r15](%1)                             \n"
           "mov %c[r15](%0), %%r15      #restore r15-r12       \n"
           "mov %c[r14](%0), %%r14                             \n"
           "mov %c[r13](%0), %%r13                             \n"
           "mov %c[r12](%0), %%r12                             \n"
           "mov %c[rbx](%0), %%rbx      #restore rbx           \n"
           "mov %c[rbp](%0), %%rbp      #restore frame pointer \n"
           "mov %c[rsp](%0), %%rsp      #restore stack pointer \n"
           "mov %c[rip](%0), %%rax      #restore frame pointer \n"
           "mov %%rax, (%%rsp)                                 \n"
           "ret                                                \n"
           :"=D"(to)
           :"S"(from),
            [rsp]"i"(__builtin_offsetof(ctx, rsp)),
            [rbp]"i"(__builtin_offsetof(ctx, rbp)),
            [rip]"i"(__builtin_offsetof(ctx, rip)),
            [rbx]"i"(__builtin_offsetof(ctx, rbx)),
            [r12]"i"(__builtin_offsetof(ctx, r12)),
            [r13]"i"(__builtin_offsetof(ctx, r13)),
            [r14]"i"(__builtin_offsetof(ctx, r13)),
            [r15]"i"(__builtin_offsetof(ctx, r15))
           :"rax"
           );
#endif
}
