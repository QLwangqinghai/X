//
//  XNumber.c
//  
//
//  Created by vector on 2020/4/13.
//

#include "XRef.h"
#include "internal/XRuntimeInternal.h"
#include "internal/XClass.h"


/*
Tagged object
Date
32位：
refType: 1, value = 1
taggedContent: {
   class: 2
   objectContent: 28 {
       unit: 2
       numberContent: 26
   }
}
 
64位：
refType: 1, value = 1
taggedContent: {
   class: 2
   objectContent: 60 {
       unit: 1
       numberContent: 59
   }
}
*/


#define X_BUILD_TaggedDateMillisecondMin (XTimeIntervalMin / 1000)
#define X_BUILD_TaggedDateMillisecondMax (XTimeIntervalMax / 1000)
#define X_BUILD_TaggedDateSecondMin (XTimeIntervalMin / 1000000)
#define X_BUILD_TaggedDateSecondMax (XTimeIntervalMax / 1000000)


#if CX_TARGET_RT_64_BIT

#define X_BUILD_TaggedDateUnitMask 0x1000000000000000ULL
#define X_BUILD_TaggedDateUnitFlagMillisecond 0x1000000000000000ULL

#define X_BUILD_TaggedDateMax  0x3FFFFFFFFFFFFFFLL
#define X_BUILD_TaggedDateMin -0x400000000000000LL
#define X_BUILD_TaggedDateContentMask 0x7FFFFFFFFFFFFFFULL

#define X_BUILD_TaggedDateContentSignBit    0x400000000000000ULL
#define X_BUILD_TaggedDateContentSignHigh  0xF800000000000000ULL
    
#else

#define X_BUILD_TaggedDateUnitMask 0x18000000UL
#define X_BUILD_TaggedDateUnitFlagMillisecond 0x8000000UL
#define X_BUILD_TaggedDateUnitFlagSecond 0x10000000UL

#define X_BUILD_TaggedDateMax  0x1FFFFFFLL
#define X_BUILD_TaggedDateMin -0x2000000LL
#define X_BUILD_TaggedDateContentMask 0x3FFFFFFUL

#define X_BUILD_TaggedDateContentSignBit    0x2000000UL
#define X_BUILD_TaggedDateContentSignHigh  0xFC000000UL

#endif


const _XDate _XDateMin = {
    ._runtime = _XConstantObjectCompressedBaseMake(X_BUILD_CompressedType_Date),
    .content = {
        .time = XTimeIntervalMin,
    },
};
const _XDate _XDateMax = {
    ._runtime = _XConstantObjectCompressedBaseMake(X_BUILD_CompressedType_Date),
    .content = {
        .time = XTimeIntervalMax,
    },
};

const XDate _Nonnull XDateMin = (XDate)(&_XDateMin);
const XDate _Nonnull XDateMax = (XDate)(&_XDateMax);

static XDate _Nonnull _XHeapDateCreate(XUInt flag, XTimeInterval time) {
    return NULL;
}





//2bit type, 有效位数 64(32) - 6 = 58(26), type = 0 标识微妙、 type = 1标识毫秒 type=2标识秒
XDate _Nonnull XDateCreate(XUInt flag, XTimeInterval time) {
    if (XTimeIntervalMin == time) {
        return XDateMin;
    } else if (XTimeIntervalMax == time) {
        return XDateMax;
    }

#if CX_TARGET_RT_64_BIT
    if (X_BUILD_TaggedDateMin <= time && time <= X_BUILD_TaggedDateMax) {
        XSInt64 value = time;
        XUInt64 tmp = *(XUInt64 *)(&value);
        XUInt64 content = (tmp & X_BUILD_TaggedDateContentMask) << 1ULL;
        XUInt64 result = X_BUILD_TaggedObjectFlag | X_BUILD_TaggedObjectClassDate | content;
        XNumber ref = (XNumber)((uintptr_t)result);
        XAssert((XUInt64)((uintptr_t)ref) == result, __func__, "unknown error");
        return ref;
    } else {
        if (time % 1000LL == 0) {//可以用毫秒秒作为单位 type = 1
            XSInt64 value = time / 1000LL;
            if (X_BUILD_TaggedDateMin <= value && value <= X_BUILD_TaggedDateMax) {
                XUInt64 tmp = *(XUInt64 *)(&value);
                XUInt64 content = (tmp & X_BUILD_TaggedDateContentMask) << 1ULL;
                XUInt64 result = X_BUILD_TaggedObjectFlag | X_BUILD_TaggedObjectClassDate | X_BUILD_TaggedDateUnitFlagMillisecond | content;
                XNumber ref = (XNumber)((uintptr_t)result);
                XAssert((XUInt64)((uintptr_t)ref) == result, __func__, "unknown error");
                return ref;
            }
        }
    }
#else
    if (X_BUILD_TaggedDateMin <= time && time <= X_BUILD_TaggedDateMax) {
        XSInt32 value = time;
        XUInt32 tmp = *(XUInt32 *)(&value);
        XUInt32 content = (tmp & X_BUILD_TaggedDateContentMask) << 1UL;
        XUInt32 result = X_BUILD_TaggedObjectFlag | X_BUILD_TaggedObjectClassDate | content;
        XNumber ref = (XNumber)((uintptr_t)result);
        XAssert((XUInt32)((uintptr_t)ref) == result, __func__, "unknown error");
        return ref;
    } else {
        if (time % 1000000LL == 0) {//可以用秒作为单位 type = 2
            XSInt64 value = time / 1000000LL;
            if (X_BUILD_TaggedDateMin <= value && value <= X_BUILD_TaggedDateMax) {
                XSInt32 value32 = (XSInt32)value;
                XUInt32 tmp = *(XUInt32 *)(&value32);
                XUInt32 content = (tmp & X_BUILD_TaggedDateContentMask) << 1ULL;
                XUInt32 result = X_BUILD_TaggedObjectFlag | X_BUILD_TaggedObjectClassDate | X_BUILD_TaggedDateUnitFlagSecond | content;
                XNumber ref = (XNumber)((uintptr_t)result);
                XAssert((XUInt32)((uintptr_t)ref) == result, __func__, "unknown error");
                return ref;
            }
        } else if (time % 1000LL == 0) {//可以用毫秒秒作为单位 type = 1
            XSInt64 value = time / 1000LL;
            if (X_BUILD_TaggedDateMin <= value && value <= X_BUILD_TaggedDateMax) {
                XSInt32 value32 = (XSInt32)value;
                XUInt32 tmp = *(XUInt32 *)(&value32);
                XUInt32 content = (tmp & X_BUILD_TaggedDateContentMask) << 1ULL;
                XUInt32 result = X_BUILD_TaggedObjectFlag | X_BUILD_TaggedObjectClassDate | X_BUILD_TaggedDateUnitFlagMillisecond | content;
                XNumber ref = (XNumber)((uintptr_t)result);
                XAssert((XUInt32)((uintptr_t)ref) == result, __func__, "unknown error");
                return ref;
            }
        }
    }

#endif
    return _XHeapDateCreate(flag, time);
}

static _XDate * _Nonnull __XRefAsDate(XDate _Nonnull ref, const char * _Nonnull func) {
    XIndex typeId = _XHeapRefGetTypeId(ref);
    XAssert(X_BUILD_TypeId_Date == typeId, func, "not Date instance");
    return (_XDate *)ref;
}

void __XDateUnpack(XDate _Nonnull ref, XTimeInterval * _Nonnull valuePtr, const char * _Nonnull func) {
    XTaggedType type = _XRefGetTaggedType(ref);
    if (type > XTaggedTypeMax) {
        _XDate * date = __XRefAsDate(ref, func);
        XTimeInterval content = date->content.time;
        *valuePtr = content;
        return;
    }
    XAssert(XTaggedTypeDate == type, func, "not Date instance");

#if CX_TARGET_RT_64_BIT
    XUInt64 v = (XUInt64)((uintptr_t)ref);
    XUInt64 tmp = (v >> 1) & X_BUILD_TaggedDateContentMask;
    XUInt64 bits = 0;
    if (tmp & X_BUILD_TaggedDateContentSignBit) {
        bits = X_BUILD_TaggedDateContentSignHigh | tmp;
    } else {
        bits = tmp;
    }
    XSInt64 number = *(XSInt64 *)(&bits);
    
    if (X_BUILD_TaggedDateUnitFlagMillisecond == (X_BUILD_TaggedDateUnitMask & v)) {
        XAssert(number >= X_BUILD_TaggedDateMillisecondMin, func, "date object error 1");
        XAssert(number <= X_BUILD_TaggedDateMillisecondMax, func, "date object error 2");
        number = number * 1000;
    }
    
    XTimeInterval content = (XTimeInterval)number;
    *valuePtr = content;
    return;
#else
    XUInt32 v = (XUInt32)((uintptr_t)ref);
    XUInt32 tmp = (v >> 1) & X_BUILD_TaggedDateContentMask;
    XUInt32 bits = 0;
    if (tmp & X_BUILD_TaggedDateContentSignBit) {
        bits = X_BUILD_TaggedDateContentSignHigh | tmp;
    } else {
        bits = tmp;
    }
    XSInt32 number32 = *(XSInt32 *)(&bits);
    XSInt64 number64 = (XSInt64)number32;

    XUInt32 unit = (X_BUILD_TaggedDateUnitMask & v);
    if (0 != unit) {
        if (X_BUILD_TaggedDateUnitFlagMillisecond == unit) {
            XAssert(number >= X_BUILD_TaggedDateMillisecondMin, func, "date object error 1");
            XAssert(number <= X_BUILD_TaggedDateMillisecondMax, func, "date object error 2");
            number = number * 1000;
        } else if (X_BUILD_TaggedDateUnitFlagSecond == unit) {
            XAssert(number >= X_BUILD_TaggedDateSecondMin, func, "date object error 1");
            XAssert(number <= X_BUILD_TaggedDateSecondMax, func, "date object error 2");
            number = number * 1000000;
        }
    }

    XTimeInterval content = (XTimeInterval)number64;
    *valuePtr = content;
    return;
#endif

}

XTimeInterval XDateGetValue(XDate _Nonnull ref) {
    XAssert(NULL != ref, __func__, "ref is NULL");
    XTimeInterval content = 0;
    __XDateUnpack(ref, &content, __func__);
    return content;
}

XComparisonResult XDateCompare(XNumber _Nonnull lhs, XNumber _Nonnull rhs) {
    XTimeInterval contentOfLhs = 0;
    XTimeInterval contentOfRhs = 0;
    __XDateUnpack(lhs, &contentOfLhs, __func__);
    __XDateUnpack(rhs, &contentOfRhs, __func__);
    if (contentOfLhs == contentOfRhs) {
        return XCompareEqualTo;
    } else if (contentOfLhs < contentOfRhs) {
        return XCompareLessThan;
    } else {
        return XCompareGreaterThan;
    }
}


XBool XDateEqual(XRef _Nonnull lhs, XRef _Nonnull rhs) {
    XAssert(NULL != lhs, __func__, "lhs is NULL");
    XAssert(NULL != rhs, __func__, "rhs is NULL");

    XTimeInterval contentOfLhs = 0;
    XTimeInterval contentOfRhs = 0;
    __XDateUnpack(lhs, &contentOfLhs, __func__);
    __XDateUnpack(rhs, &contentOfRhs, __func__);
    XBool b = contentOfLhs == contentOfRhs;
    return b;
}

XHashCode XDateHash(XRef _Nonnull ref) {
    XAssert(NULL != ref, __func__, "ref is NULL");
    XTimeInterval content = 0;
    __XDateUnpack(ref, &content, __func__);
    return _XHashSInt64(content);
}
