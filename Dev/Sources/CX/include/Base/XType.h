//
//  XType.h
//  X
//
//  Created by vector on 2020/4/3.
//  Copyright © 2020 haoqi. All rights reserved.
//

#ifndef XType_h
#define XType_h


#if defined(__cplusplus)
extern "C" {
#endif


#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h>
#include <stdint.h>
#include <unistd.h>
#include <assert.h>
#include <wchar.h>
#include <time.h>
#include <limits.h>
#include <math.h>
#include <string.h>
#include <errno.h>
#include <stddef.h>

#if defined(__GNUC__)
#include <stdbool.h>
#endif
#include "CXTargetConditionals.h"

#define XAssert(cond, func, desc) {\
    if (!(cond)) {\
        fprintf(stderr, "%s error, %s\n", func, desc);\
        abort();\
    }\
}


typedef _Bool XBool;

typedef uint8_t XUInt8;
typedef uint16_t XUInt16;
typedef uint32_t XUInt32;
typedef uint64_t XUInt64;

typedef int8_t XSInt8;
typedef int16_t XSInt16;
typedef int32_t XSInt32;
typedef int64_t XSInt64;

typedef float XFloat32;
typedef double XFloat64;

typedef uint8_t XByte;
typedef size_t XSize;
typedef uintptr_t XUIntptr;
typedef intptr_t XSIntptr;


#if CX_TARGET_RT_64_BIT
    typedef int64_t XSInt;
    typedef uint64_t XUInt;

    #define XSIntBitsCount 64
    #define XUIntBitsCount 64
    #define XUIntHalfBitsCount 32

    #define XSIntMin INT64_MIN
    #define XSIntMax INT64_MAX

    #define XUIntMax UINT64_MAX

    #define X_BUILD_UInt(value) UINT64_C(value)

#else
    typedef int32_t XSInt;
    typedef uint32_t XUInt;

    #define XSIntBitsCount 32
    #define XUIntBitsCount 32
    #define XUIntHalfBitsCount 16

    #define XSIntMin INT32_MIN
    #define XSIntMax INT32_MAX

    #define XUIntMax UINT32_MAX
    #define X_BUILD_UInt(value) UINT32_C(value)

#endif
    

    

typedef XUInt XIndex;
//#define XIndexNotFound XUIntMax
static const XIndex XIndexNotFound = XUIntMax;
typedef struct _XRange {
    XIndex location;
    XIndex length;
} XRange;
/*
 应该优先判断
 (XIndexNotFound != location)
 */


static inline XRange XRangeMake(XIndex location, XIndex length) {
    XRange r = {
        .location = location,
        .length = length,
    };
    return r;
}

//向前移动
static inline XRange XRangeMoveForward(XRange range, XIndex n) {
    XAssert(range.location >= n, __func__, "");
    XRange r = {
        .location = range.location - n,
        .length = range.length,
    };
    return r;
}
//向后移动
static inline XRange XRangeMoveBackward(XRange range, XIndex n) {
    XAssert(range.location + n >= range.location, __func__, "");
    XRange r = {
        .location = range.location + n,
        .length = range.length,
    };
    return r;
}
static inline XRange XRangeMove(XRange range, XSInt n) {
    if (n < 0) {
        return XRangeMoveForward(range, - n);
    } else {
        return XRangeMoveBackward(range, n);
    }
}


typedef char XU8Char;
typedef uint16_t XU16Char;
typedef uint32_t XU32Char;

#pragma mark - pointer

typedef void * XPtr;

#pragma mark - XHashCode

typedef XUInt XHashCode;

#pragma mark - XTimeInterval

//微秒时间
typedef int64_t XTimeInterval;
#define XTimeIntervalMin INT64_MIN
#define XTimeIntervalMax INT64_MAX

#define XUInt8Max UINT8_MAX
#define XUInt16Max UINT16_MAX
#define XUInt32Max UINT32_MAX
#define XUInt64Max UINT64_MAX

#define XSInt8Max INT8_MAX
#define XSInt16Max INT16_MAX
#define XSInt32Max INT32_MAX
#define XSInt64Max INT64_MAX

#define XSInt8Min INT8_MIN
#define XSInt16Min INT16_MIN
#define XSInt32Min INT32_MIN
#define XSInt64Min INT64_MIN

    
#define XFastSInt32 int_fast32_t
#define XFastUInt32 uint_fast32_t
    
#define XFastSInt64 int_fast64_t
#define XFastUInt64 uint_fast64_t
    
#if CX_TARGET_RT_64_BIT
#define XFastSInt int_fast64_t
#define XFastUInt uint_fast64_t
    
#define XFastSIntMin INT_FAST64_MIN
#define XFastSIntMax INT_FAST64_MAX
    
#define XFastUIntMax UINT_FAST64_MAX
    
#else
#define XFastSInt int_fast32_t
#define XFastUInt uint_fast32_t
    
#define XFastSIntMin INT_FAST32_MIN
#define XFastSIntMax INT_FAST32_MAX
    
#define XFastUIntMax UINT_FAST32_MAX
    
#endif

    
typedef XSInt32 XComparisonResult;

// < 0
extern const XComparisonResult XCompareLessThan;

// = 0
extern const XComparisonResult XCompareEqualTo;

// > 0
extern const XComparisonResult XCompareGreaterThan;

#pragma mark - XByte

//used for uuid、md5、ipAddress
typedef struct {
    uint8_t content[16];
} XByte16;

static inline XBool XByte16Equal(const XByte16 * _Nonnull lhs, const XByte16 * _Nonnull rhs) {
    assert(lhs);
    assert(rhs);
    return memcmp(lhs, rhs, sizeof(XByte16)) == 0;
}
static inline void XByte16ToBytes(const XByte16 * _Nonnull bytes, XByte * _Nonnull buffer) {
    assert(bytes);
    memcpy(buffer, bytes->content, 16);
}
static inline void XByte16Reset(XByte16 * _Nonnull bytes, const XByte * _Nonnull buffer) {
    assert(bytes);
    memcpy(bytes->content, buffer, 16);
}

typedef struct {
    uint8_t content[20];
} XByte20;
static inline XBool XByte20Equal(const XByte20 * _Nonnull lhs, const XByte20 * _Nonnull rhs) {
    assert(lhs);
    assert(rhs);
    return memcmp(lhs, rhs, sizeof(XByte20)) == 0;
}
static inline void XByte20ToBytes(const XByte20 * _Nonnull bytes, XByte * _Nonnull buffer) {
    assert(bytes);
    memcpy(buffer, bytes->content, 20);
}
static inline void XByte20Reset(XByte20 * _Nonnull bytes, const XByte * _Nonnull buffer) {
    assert(bytes);
    memcpy(bytes->content, buffer, 20);
}

typedef struct {
    uint8_t content[32];
} XByte32;
static inline XBool XByte32Equal(const XByte32 * _Nonnull lhs, const XByte32 * _Nonnull rhs) {
    assert(lhs);
    assert(rhs);
    return memcmp(lhs, rhs, sizeof(XByte32)) == 0;
}
static inline void XByte32ToBytes(const XByte32 * _Nonnull bytes, XByte * _Nonnull buffer) {
    assert(bytes);
    memcpy(buffer, bytes->content, 32);
}
static inline void XByte32Reset(XByte32 * _Nonnull bytes, const XByte * _Nonnull buffer) {
    assert(bytes);
    memcpy(bytes->content, buffer, 32);
}

typedef struct {
    uint8_t content[48];
} XByte48;
static inline XBool XByte48Equal(const XByte48 * _Nonnull lhs, const XByte48 * _Nonnull rhs) {
    assert(lhs);
    assert(rhs);
    return memcmp(lhs, rhs, sizeof(XByte48)) == 0;
}
static inline void XByte48ToBytes(const XByte48 * _Nonnull bytes, XByte * _Nonnull buffer) {
    assert(bytes);
    memcpy(buffer, bytes->content, 48);
}
static inline void XByte48Reset(XByte48 * _Nonnull bytes, const XByte * _Nonnull buffer) {
    assert(bytes);
    memcpy(bytes->content, buffer, 48);
}

typedef struct {
    uint8_t content[64];
} XByte64;
static inline XBool XByte64Equal(const XByte64 * _Nonnull lhs, const XByte64 * _Nonnull rhs) {
    assert(lhs);
    assert(rhs);
    return memcmp(lhs, rhs, sizeof(XByte64)) == 0;
}
static inline void XByte64ToBytes(const XByte64 * _Nonnull bytes, uint8_t * _Nonnull buffer) {
    assert(bytes);
    memcpy(buffer, bytes->content, 64);
}
static inline void XByte64Reset(XByte64 * _Nonnull bytes, const uint8_t * _Nonnull buffer) {
    assert(bytes);
    memcpy(bytes->content, buffer, 64);
}

typedef struct {
    uint8_t content[128];
} XByte128;
static inline XBool XByte128Equal(const XByte128 * _Nonnull lhs, const XByte128 * _Nonnull rhs) {
    assert(lhs);
    assert(rhs);
    return memcmp(lhs, rhs, sizeof(XByte128)) == 0;
}
static inline void XByte128ToBytes(const XByte128 * _Nonnull bytes, uint8_t * _Nonnull buffer) {
    assert(bytes);
    memcpy(buffer, bytes->content, 128);
}
static inline void XByte128Reset(XByte128 * _Nonnull bytes, const uint8_t * _Nonnull buffer) {
    assert(bytes);
    memcpy(bytes->content, buffer, 128);
}





//typedef enum {
//    XTypeVoid = 0x0,
//    XTypeBool = 0x1,
//    XTypeSInt8 = 0x2,
//    XTypeUInt8 = 0x3,
//    XTypeSInt16 = 0x4,
//    XTypeUInt16 = 0x5,
//    XTypeSInt32 = 0x6,
//    XTypeUInt32 = 0x7,
//    XTypeSInt64 = 0x8,
//    XTypeUInt64 = 0x9,
//    XTypeFloat32 = 0xA,
//    XTypeFloat64 = 0xB,
//    XTypeRef = 0xC,
//
//
//    XTypeVoidPtr = 0x10,
//    XTypeBoolPtr = 0x11,
//    XTypeSInt8Ptr = 0x12,
//    XTypeUInt8Ptr = 0x13,
//    XTypeSInt16Ptr = 0x14,
//    XTypeUInt16Ptr = 0x15,
//    XTypeSInt32Ptr = 0x16,
//    XTypeUInt32Ptr = 0x17,
//    XTypeSInt64Ptr = 0x18,
//    XTypeUInt64Ptr = 0x19,
//    XTypeFloat32Ptr = 0x1A,
//    XTypeFloat64Ptr = 0x1B,
//    XTypeRefPtr = 0x1C,
//} XTypeId_e;

//typedef enum {
//    XTypeNone = 0,
//    XTypeNumber = 1,
//    XTypeString = 2,
//    XTypeDate = 3,
//    XTypeData = 4,
//    XTypeNull = 5,
//    XTypeArray = 6,
//    XTypeMap = 7,
//} XType_e;
//
//static inline _Bool XTypeIsValid(XType_e t) {
//    return (t >= XTypeBool && t <= XTypeBuffer);
//}




#if defined(__cplusplus)
}  // extern C
#endif

#endif /* XType_h */
