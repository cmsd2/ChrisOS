/*-
 * Copyright (c) 1982, 1986, 1989, 1993
 *	The Regents of the University of California.  All rights reserved.
 * (c) UNIX System Laboratories, Inc.
 * All or some portions of this file are derived from material licensed
 * to the University of California by American Telephone and Telegraph
 * Co. or Unix System Laboratories, Inc. and are reproduced herein with
 * the permission of UNIX System Laboratories, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *	@(#)param.h	8.3 (Berkeley) 4/4/95
 * $FreeBSD$
 */

#ifndef _SYS_PARAM_H_
#define _SYS_PARAM_H_

#include <sys/_null.h>

#include <sys/syslimits.h>
#include <arch/param.h>
#include <sys/types.h>

#ifndef _KERNEL
#include <sys/limits.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif
    
#define	NBBY	8		/* number of bits in a byte */
#define	NBPW	sizeof(int)	/* number of bytes per word (integer) */

/* Bit map related macros. */
#define	setbit(a,i)	(((unsigned char *)(a))[(i)/NBBY] |= 1<<((i)%NBBY))
#define	clrbit(a,i)	(((unsigned char *)(a))[(i)/NBBY] &= ~(1<<((i)%NBBY)))
#define	isset(a,i)							\
	(((const unsigned char *)(a))[(i)/NBBY] & (1<<((i)%NBBY)))
#define	isclr(a,i)							\
	((((const unsigned char *)(a))[(i)/NBBY] & (1<<((i)%NBBY))) == 0)

// 0 <= a <= 31, 0 <= b <= 31
#define bitmask(a, b) (((unsigned) -1 >> (31 - (b))) & ~((1U << (a)) - 1))
#define getbit(a, i) getbits(a, i, i)
#define getbits(a, i, j) ((a & bitmask(i, j)) >> i)
#define withbit(a, i, value) withbits(a, i, i, value)
#define withbits(a, i, j, value) ((a & ~bitmask(i, j)) | ((value << i) & bitmask(i, j)))

/* Macros for counting and rounding. */
#ifndef howmany
#define	howmany(x, y)	(((x)+((y)-1))/(y))
#endif
#define	nitems(x)	(sizeof((x)) / sizeof((x)[0]))
#define	rounddown(x, y)	(((x)/(y))*(y))
#define	rounddown2(x, y) ((x)&(~((y)-1)))          /* if y is power of two */
#define	roundup(x, y)	((((x)+((y)-1))/(y))*(y))  /* to any y */
#define	roundup2(x, y)	(((x)+((y)-1))&(~((y)-1))) /* if y is powers of two */
#define powerof2(x)	((((x)-1)&(x))==0)

/* Macros for min/max. */
#ifndef MAX
#define MAX(a,b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a > _b ? _a : _b; })
#endif

#ifndef MIN
#define MIN(a,b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a < _b ? _a : _b; })
#endif

#define LOW_BYTE(word) ((word)&0xff)
#define HIGH_BYTE(word) LOW_BYTE((word)>>8)

#ifdef _KERNEL
/*
 * Basic byte order function prototypes for non-inline functions.
 */
#ifndef LOCORE
#ifndef _BYTEORDER_PROTOTYPED
#define	_BYTEORDER_PROTOTYPED
__BEGIN_DECLS
__uint32_t	 htonl(__uint32_t);
__uint16_t	 htons(__uint16_t);
__uint32_t	 ntohl(__uint32_t);
__uint16_t	 ntohs(__uint16_t);
__END_DECLS
#endif
#endif

#ifndef lint
#ifndef _BYTEORDER_FUNC_DEFINED
#define	_BYTEORDER_FUNC_DEFINED
#define	htonl(x)	__htonl(x)
#define	htons(x)	__htons(x)
#define	ntohl(x)	__ntohl(x)
#define	ntohs(x)	__ntohs(x)
#endif /* !_BYTEORDER_FUNC_DEFINED */
#endif /* lint */
#endif /* _KERNEL */

#ifdef __cplusplus
}
#endif

#endif	/* _SYS_PARAM_H_ */
