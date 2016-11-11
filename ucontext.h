#ifndef UCONTEXT_H
#define UCONTEXT_H

#include <windows.h>

#if defined(_X86_)
#define DUMMYARGS
#else
#define DUMMYARGS long dummy0, long dummy1, long dummy2, long dummy3, 
#endif

#define malloc(x)	_aligned_malloc(x,64)

typedef struct __stack {
	void *ss_sp;
	size_t ss_size;
	int ss_flags;
} stack_t;

typedef CONTEXT mcontext_t;
typedef unsigned long __sigset_t;

typedef struct __ucontext {
	unsigned long int	uc_flags;
	struct __ucontext	*uc_link;
	stack_t				uc_stack;
	mcontext_t			uc_mcontext;
	__sigset_t			uc_sigmask;
} ucontext_t;

int getcontext(ucontext_t *ucp);
int setcontext(const ucontext_t *ucp);
int makecontext(ucontext_t *, void(*)(), int, ...);
int swapcontext(ucontext_t *, const ucontext_t *);

#endif /* UCONTEXT_H */