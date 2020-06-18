//
//  CXTargetConditionals.h
//  CCFoundation
//
//  Created by vector on 2018/11/26.
//  Copyright © 2018 com.wangqinghai. All rights reserved.
//

#ifndef CXTargetConditionals_h
#define CXTargetConditionals_h

#if defined(__cplusplus)
extern "C" {
#endif

#ifdef __APPLE__
#include <TargetConditionals.h>
#include <AvailabilityMacros.h>

#define CX_TARGET_OS_MACOS   TARGET_OS_OSX
#define CX_TARGET_OS_IOS     TARGET_OS_IOS
#define CX_TARGET_OS_WATCH     TARGET_OS_WATCH

#else

#define CX_TARGET_OS_MACOS   0
#define CX_TARGET_OS_IOS     0
#define CX_TARGET_OS_WATCH     0

#endif

#ifdef __linux__
#include <linux/version.h>
#include <features.h>
#endif

#if defined(__unix__) || (defined(__APPLE__) && defined(__MACH__))
#include <unistd.h>
#endif
#ifdef __ANDROID__
#include <linux/sysctl.h>
#else
#include <sys/sysctl.h>
#endif /* __ANDROID__ */

#if __APPLE__
#define CX_TARGET_OS_DARWIN       1
#define CX_TARGET_OS_LINUX        0
#define CX_TARGET_OS_WINDOWS      0
#define CX_TARGET_OS_BSD          0
#define CX_TARGET_OS_ANDROID      0
#elif __ANDROID__
#define CX_TARGET_OS_DARWIN       0
#define CX_TARGET_OS_LINUX        1
#define CX_TARGET_OS_WINDOWS      0
#define CX_TARGET_OS_BSD          0
#define CX_TARGET_OS_ANDROID      1
#elif __linux__
#define CX_TARGET_OS_DARWIN       0
#define CX_TARGET_OS_LINUX        1
#define CX_TARGET_OS_WINDOWS      0
#define CX_TARGET_OS_BSD          0
#define CX_TARGET_OS_ANDROID      0
#elif _WIN32 || _WIN64
#define CX_TARGET_OS_DARWIN       0
#define CX_TARGET_OS_LINUX        0
#define CX_TARGET_OS_WINDOWS      1
#define CX_TARGET_OS_BSD          0
#define CX_TARGET_OS_ANDROID      0
#elif __unix__
#define CX_TARGET_OS_DARWIN       0
#define CX_TARGET_OS_LINUX        0
#define CX_TARGET_OS_WINDOWS      0
#define CX_TARGET_OS_BSD          1
#define CX_TARGET_OS_ANDROID      0
#else
#error unknown operating system
#endif


#if (defined(__CYGWIN32__) || defined(_WIN32)) && !defined(__WIN32__)
#define __WIN32__ 1
#endif

#if defined(_WIN64) && !defined(__WIN64__)
#define __WIN64__ 1
#endif

#if defined(__WIN64__) && !defined(__LLP64__)
#define __LLP64__ 1
#endif

#if defined(_MSC_VER) && defined(_M_IX86)
#define __i386__ 1
#endif

#if (defined(__i386__) || defined(__x86_64__)) && !defined(__LITTLE_ENDIAN__)
#define __LITTLE_ENDIAN__ 1
#endif

#if !defined(__BIG_ENDIAN__) && !defined(__LITTLE_ENDIAN__)
#error Do not know the endianess of this architecture
#endif

#if !__BIG_ENDIAN__ && !__LITTLE_ENDIAN__
#error Both __BIG_ENDIAN__ and __LITTLE_ENDIAN__ cannot be false
#endif

#if __BIG_ENDIAN__ && __LITTLE_ENDIAN__
#error Both __BIG_ENDIAN__ and __LITTLE_ENDIAN__ cannot be true
#endif

/* Test for proc filesystem */
#ifdef __linux__

#ifndef HAVE_PROC_STAT
#define HAVE_PROC_STAT 1
#endif

#ifndef HAVE_PROC_MAPS
#define HAVE_PROC_MAPS 1
#endif

#ifndef HAVE_PROC_SMAPS
#define HAVE_PROC_SMAPS 1
#endif

#ifndef HAVE_PROC_SOMAXCONN
#define HAVE_PROC_SOMAXCONN 1
#endif

#endif

/* Test for task_info() */
#if defined(__APPLE__)

#ifndef HAVE_TASKINFO
#define HAVE_TASKINFO 1
#endif

#endif

/* Test for backtrace() */
#if defined(__APPLE__) || (defined(__linux__) && defined(__GLIBC__))

#ifndef HAVE_BACKTRACE
#define HAVE_BACKTRACE 1
#endif

#endif

/* MSG_NOSIGNAL. */
#ifdef __linux__

#ifndef HAVE_MSG_NOSIGNAL
#define HAVE_MSG_NOSIGNAL 1
#endif

#endif

/* Test for polling API */
#ifdef __linux__

#ifndef EVENT_HAVE_EPOLL
#define EVENT_HAVE_EPOLL 1
#endif

#endif

#if (defined(__APPLE__) && defined(MAC_OS_X_VERSION_10_6)) || defined(__FreeBSD__) || defined(__OpenBSD__) || defined (__NetBSD__)

#ifndef EVENT_HAVE_KQUEUE
#define EVENT_HAVE_KQUEUE 1
#endif

#endif


/* Byte ordering detection */
#include <sys/types.h> /* This will likely define BYTE_ORDER */

#ifndef BYTE_ORDER
#if (BSD >= 199103)
# include <machine/endian.h>
#else
#if defined(linux) || defined(__linux__)
# include <endian.h>
#else

#define    LITTLE_ENDIAN    1234    /* least-significant byte first (vax, pc) */
#define    BIG_ENDIAN    4321    /* most-significant byte first (IBM, net) */
#define    PDP_ENDIAN    3412    /* LSB first in word, MSW first in long (pdp)*/

#if defined(__i386__) || defined(__x86_64__) || defined(__amd64__) || \
defined(vax) || defined(ns32000) || defined(sun386) || \
defined(MIPSEL) || defined(_MIPSEL) || defined(BIT_ZERO_ON_RIGHT) || \
defined(__alpha__) || defined(__alpha)
#define BYTE_ORDER    LITTLE_ENDIAN
#endif

#if defined(sel) || defined(pyr) || defined(mc68000) || defined(sparc) || \
defined(is68k) || defined(tahoe) || defined(ibm032) || defined(ibm370) || \
defined(MIPSEB) || defined(_MIPSEB) || defined(_IBMR2) || defined(DGUX) ||\
defined(apollo) || defined(__convex__) || defined(_CRAY) || \
defined(__hppa) || defined(__hp9000) || \
defined(__hp9000s300) || defined(__hp9000s700) || \
defined (BIT_ZERO_ON_LEFT) || defined(m68k) || defined(__sparc)
#define BYTE_ORDER    BIG_ENDIAN
#endif
#endif /* linux */
#endif /* BSD */
#endif /* BYTE_ORDER */

/* Sometimes after including an OS-specific header that defines the
 * endianess we end with __BYTE_ORDER but not with BYTE_ORDER that is what
 * the Redis code uses. In this case let's define everything without the
 * underscores. */
#ifndef BYTE_ORDER
#ifdef __BYTE_ORDER
#if defined(__LITTLE_ENDIAN) && defined(__BIG_ENDIAN)
#ifndef LITTLE_ENDIAN
#define LITTLE_ENDIAN __LITTLE_ENDIAN
#endif
#ifndef BIG_ENDIAN
#define BIG_ENDIAN __BIG_ENDIAN
#endif
#if (__BYTE_ORDER == __LITTLE_ENDIAN)
#define BYTE_ORDER LITTLE_ENDIAN
#else
#define BYTE_ORDER BIG_ENDIAN
#endif
#endif
#endif
#endif

#if !defined(BYTE_ORDER) || \
(BYTE_ORDER != BIG_ENDIAN && BYTE_ORDER != LITTLE_ENDIAN)
/* you must determine what the correct bit order is for
 * your compiler - the next line is an intentional error
 * which will force your compiles to bomb until you fix
 * the above macros.
 */
#error "Undefined or invalid BYTE_ORDER"
#endif

#if (__i386 || __amd64 || __powerpc__) && __GNUC__
#define GNUC_VERSION (__GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__)
#if defined(__clang__)
#define HAVE_ATOMIC
#endif
#if (defined(__GLIBC__) && defined(__GLIBC_PREREQ))
#if (GNUC_VERSION >= 40100 && __GLIBC_PREREQ(2, 6))
#define HAVE_ATOMIC
#endif
#endif
#endif


#if defined(__x86_64) || defined(_M_AMD64) || defined(_M_X64)
    #define CX_TARGET_RT_64_BIT 1
    #define CX_TARGET_RT_32_BIT 0
    #define CX_TARGET_CPU_X86_64
#elif defined(__x86) || defined(__i386) || defined(__i386__) || defined(_M_IX86)
    #define CX_TARGET_RT_64_BIT 0
    #define CX_TARGET_RT_32_BIT 1
    #define CX_TARGET_CPU_X86
#elif defined(__aarch64__)
    #define CX_TARGET_RT_64_BIT 1
    #define CX_TARGET_RT_32_BIT 0
    #define CX_TARGET_CPU_AARCH64
#elif defined(__arm) || defined(__arm__) || defined(_M_ARM)
    #define CX_TARGET_RT_64_BIT 0
    #define CX_TARGET_RT_32_BIT 1
    #define CX_TARGET_CPU_ARM
#elif (defined(__PPC64__) || defined(__powerpc64__)) && defined(_LITTLE_ENDIAN)
    #define CX_TARGET_RT_64_BIT 1
    #define CX_TARGET_RT_32_BIT 0
    #define CX_TARGET_CPU_PPC64LE
#elif defined(__mips__) && !defined(__LP64__)
    #define CX_TARGET_RT_64_BIT 0
    #define CX_TARGET_RT_32_BIT 1
    #define CX_TARGET_CPU_MIPS
#elif defined(__mips__) && defined(__LP64__)
    #define CX_TARGET_RT_64_BIT 1
    #define CX_TARGET_RT_32_BIT 0
    #define CX_TARGET_CPU_MIPS64
#elif defined(__pnacl__)
    #define CX_TARGET_RT_64_BIT 0
    #define CX_TARGET_RT_32_BIT 1
    #define CX_TARGET_CPU_PNACL
#elif defined(__wasm__)
    #define CX_TARGET_RT_64_BIT 0
    #define CX_TARGET_RT_32_BIT 1
#elif defined(__asmjs__)
    #define CX_TARGET_RT_64_BIT 0
    #define CX_TARGET_RT_32_BIT 1

#elif defined(__myriad2__)
    #define CX_TARGET_RT_64_BIT 0
    #define CX_TARGET_RT_32_BIT 1

#else
    #error "Unknown target CPU"
#endif













/* Make sure we can test for ARM just checking for __arm__, since sometimes
 * __arm is defined but __arm__ is not. */
#if defined(__arm) && !defined(__arm__)
#define __arm__
#endif
#if defined (__aarch64__) && !defined(__arm64__)
#define __arm64__
#endif

/* Make sure we can test for SPARC just checking for __sparc__. */
#if defined(__sparc) && !defined(__sparc__)
#define __sparc__
#endif

#if __LITTLE_ENDIAN__
#define CX_TARGET_RT_LITTLE_ENDIAN 1
#define CX_TARGET_RT_BIG_ENDIAN    0
#elif __BIG_ENDIAN__
#define CX_TARGET_RT_LITTLE_ENDIAN 0
#define CX_TARGET_RT_BIG_ENDIAN    1
#else
#error unknown endian
#endif

#if defined(__cplusplus)
}  // extern C
#endif

#endif /* CXTargetConditionals_h */
