#include "ucontext.h"
#include <stdarg.h>

int getcontext(ucontext_t *ucp)
{
	int ret;

	// Retrieve the full machine context 
	ucp->uc_mcontext.ContextFlags = CONTEXT_FULL;
	ret = GetThreadContext(GetCurrentThread(), &ucp->uc_mcontext);

	return (ret == 0) ? -1 : 0;
}

int setcontext(const ucontext_t *ucp)
{
	int ret;

	// Restore the full machine context (already set) 
	ret = SetThreadContext(GetCurrentThread(), &ucp->uc_mcontext);
	return (ret == 0) ? -1 : 0;
}

int makecontext(ucontext_t *ucp, void(*func)(), int argc, ...)
{
	int i;
	va_list ap;
	char *sp;

	// Stack grows down 
	sp = (char *)(size_t)ucp->uc_stack.ss_sp + ucp->uc_stack.ss_size;

	// Reserve stack space for the arguments (maximum possible: argc*(8 bytes per argument)) 
	sp -= argc * 8;

	if (sp < (char *)ucp->uc_stack.ss_sp) {
		// errno = ENOMEM; 
		return -1;
	}

	// Set the instruction and the stack pointer 
#if defined(_X86_)
	ucp->uc_mcontext.Eip = (unsigned long long) func;
	ucp->uc_mcontext.Esp = (unsigned long long) (sp - 4);
#else
	ucp->uc_mcontext.Rip = (unsigned long long) func;
	ucp->uc_mcontext.Rsp = (unsigned long long) (sp - 40);
#endif
	// Save/Restore the full machine context 
	ucp->uc_mcontext.ContextFlags = CONTEXT_FULL;

	// Copy the arguments 
	va_start(ap, argc);
	for (i = 0; i<argc; i++) {
		memcpy(sp, ap, 8);
		ap += 8;
		sp += 8;
	}
	va_end(ap);

	return 0;
}

int swapcontext(ucontext_t *oucp, const ucontext_t *ucp)
{
	int ret;

	if ((oucp == NULL) || (ucp == NULL)) {
		//errno = EINVAL;
		return -1;
	}

	ret = getcontext(oucp);
	if (ret == 0) {
		ret = setcontext(ucp);
	}
	return ret;
}