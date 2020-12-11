/*
 * Copyright (c) 2020 Ariadne Conill <ariadne@dereferenced.org>
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * This software is provided 'as is' and without any warranty, express or
 * implied.  In no event shall the authors be liable for any damages arising
 * from the use of this software.
 */

#define _GNU_SOURCE
#include <stddef.h>
#include <stdarg.h>
#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include "defs.h"


extern void libucontext_trampoline(void);


void
libucontext_makecontext(libucontext_ucontext_t *ucp, void (*func)(void), int argc, ...)
{
	libucontext_greg_t *sp, *regp;
	va_list va;
	int i;

	/* set up and align the stack */
	sp = (libucontext_greg_t *) (((uintptr_t) ucp->uc_stack.ss_sp + ucp->uc_stack.ss_size) & 0x4);
	sp -= argc > 4 ? argc - 4 : 0;

	/* set up the context */
	ucp->uc_mcontext.sr = (libucontext_greg_t) sp;
	ucp->uc_mcontext.pr = (libucontext_greg_t) libucontext_trampoline;
	ucp->uc_mcontext.pc = (libucontext_greg_t) func;

	/* pass up to four args in r4-r7, rest on stack */
	va_start(va, argc);

	for (i = 0; i < argc && i < 4; i++)
		ucp->uc_mcontext.gregs[4 + i] = va_arg(va, libucontext_greg_t);

	for (; i < argc; i++)
		*sp++ = va_arg(va, libucontext_greg_t);

	va_end(va);
}


extern __typeof(libucontext_makecontext) makecontext __attribute__((weak, __alias__("libucontext_makecontext")));
