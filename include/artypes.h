#ifndef ARTYPES_H
#define ARTYPES_H

#ifndef INT_MAX
/*
 * This header depends on the Standard C <limits.h> method of
 * describing the integral sizes - it needs INT_MAX and friends
 * to be defined and useful in C preprocessor arithmetic.
 * If you can't or don't want to include the standard C/C++
 * header, you can still use this file by providing your own
 * definitions for {,U}{INT,LONG,SHRT}_{MIN,MAX}.
 */
#if defined(__cplusplus)
#include <climits>
#else
#include <limits.h>
#endif
#endif  /* INT_MAX */

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif


/*
 * ARLong32 is a 32-bit signed integer on either 32-bit or
 * 64-bit machines.  It replaces "long" in the AR C API.
 * The definition is conditioned on INT_MAX for backward
 * compatibility; codes that use "long" expecting 32 bits
 * are only inconvenienced on machines where a long really
 * isn't 32 bits.
 *
 * ARLONG32_{MAX,MIN} are used analogously to the standard
 * LONG_{MAX,MIN} in parameterizing the machine word size out
 * of the code.
 *
 * ARLONG32_L can be used as a string infix to parameterize
 * word size out of calls to printf/scanf functions.  E.g.,
 *      ARLong32 bob;
 *      sscanf(input, "%"ARLONG32_L"d", &bob);  // either "%d" or "%ld"
 *
 * ARL0 parametrizes ARL(), defined below.
 *
 * ARULong32 &c. are just unsigned variants of ARLong32 &c.
 */

#if SHRT_MAX < INT_MAX && INT_MAX < LONG_MAX

typedef int ARLong32;
#define ARLONG32_MAX INT_MAX
#define ARLONG32_MIN INT_MIN
#define ARLONG32_L ""
#define ARL0
#define xdr_ARLong32 xdr_int

#else

typedef long ARLong32;
#define ARLONG32_MAX LONG_MAX
#define ARLONG32_MIN LONG_MIN
#define ARLONG32_L "l"
#define ARL0 L
#define xdr_ARLong32 xdr_long

#endif  /* SHRT_MAX... */


#if USHRT_MAX < UINT_MAX && UINT_MAX < ULONG_MAX

typedef unsigned int ARULong32;
#define ARULONG32_MAX UINT_MAX
#define ARULONG32_L ""
#define xdr_ARULong32 xdr_u_int

#else

typedef unsigned long ARULong32;
#define ARULONG32_MAX ULONG_MAX
#define ARULONG32_L "l"
#define xdr_ARULong32 xdr_u_long

#endif  /* USHRT_MAX... */


/*
 * ARL(4) -> 4L
 * ARL(4U) -> 4UL
 * ... but be careful:
 * ARL(-1U) -> -1UL, which means -(unsigned long) 1, not (unsigned long) -1.
 */
#define ARL(z)ARCC(z,ARL0)

#if !defined(ARCC)
#define ARCC(yat,yee)ARCC1(yat,yee)
#define ARCC1(yat,yee)yat##yee
#endif

#if !defined(ARQQ)
#define ARQQ(x)ARQQ1(x)
#define ARQQ1(x)#x
#endif

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#endif  /* ARTYPES_H */
