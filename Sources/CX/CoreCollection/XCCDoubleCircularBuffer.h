//
//  XCCDoubleCircularBuffer.h
//  X
//
//  Created by vector on 2020/5/31.
//  Copyright © 2020 haoqi. All rights reserved.
//

#ifndef XCCDoubleCircularBuffer_h
#define XCCDoubleCircularBuffer_h

#include "XCCBase.h"


#if CX_TARGET_RT_64_BIT

//#define X_BUILD_DoubleCircularBufferTableCapacityShift X_BUILD_UInt(20)

//2m
#define X_BUILD_DoubleCircularBufferPageCapacity X_BUILD_UInt(0x200000)

typedef struct __XCCDoubleCircularBufferIndex {
    XIndex page: 43;
    XIndex item: 21;
} XCCDoubleCircularBufferIndex;

#else

//256k
#define X_BUILD_DoubleCircularBufferPageCapacity X_BUILD_UInt(0x40000)

typedef struct __XCCDoubleCircularBufferIndex {
    XIndex page: 14;
    XIndex item: 18;
} XCCDoubleCircularBufferIndex;

#endif


static inline XCCDoubleCircularBufferIndex XCCDoubleCircularBufferIndexMake(XIndex item, XIndex page) {
    XCCDoubleCircularBufferIndex result = {};
    result.item = item;
    result.page = page;
    return result;
}

static inline XCCDoubleCircularBufferIndex XCCDoubleCircularBufferIndexMakeWithIndex(XIndex location) {
    XCCDoubleCircularBufferIndex result = {};
#if CX_TARGET_RT_64_BIT
    result.item = location & X_BUILD_UInt(0xFFFFFF);
    result.page = (location >> 21);
#else
    result.item = location & X_BUILD_UInt(0x3FFFF);
    result.page = (location >> 18);
#endif
    return result;
}

static inline XIndex XCCDoubleCircularBufferIndexToIndex(XCCDoubleCircularBufferIndex location) {
    XIndex loc = 0;
#if CX_TARGET_RT_64_BIT
    loc += location.page;
    loc = loc << 21;
    loc += location.item;
#else
    loc += location.page;
    loc = loc << 18;
    loc += location.item;
#endif
    return loc;
}

static inline XCCDoubleCircularBufferIndex __XCCDoubleCircularBufferAlignedCapacity(XIndex capacity) {
    XCCDoubleCircularBufferIndex result = {};
    if (capacity >= X_BUILD_ArrayCapacityMax) {
        result.page = (X_BUILD_ArrayCapacityMax / X_BUILD_DoubleCircularBufferPageCapacity);
    } else if (capacity > X_BUILD_DoubleCircularBufferPageCapacity / 2) {
        result.page = (capacity + X_BUILD_DoubleCircularBufferPageCapacity - 1) / X_BUILD_DoubleCircularBufferPageCapacity;
    } else {
        if (capacity > 0) {
            XIndex cc = 8;
            while (cc < capacity) {
                cc = cc << 1;
            }
            result.item = cc;
        }
    }
    return result;
}


static inline XIndex __XCCDoubleCircularBufferGoodCapacity(XIndex capacity) {
    if (capacity > X_BUILD_DoubleCircularBufferPageCapacity) {
        return X_BUILD_ArrayCapacityMax;
    } else {
        if (capacity <= 0) {
            return 0;
        } else {
            XIndex result = 8;
            while (result < capacity) {
                result = result << 1;
            }
            return result;
        }
    }
}


typedef struct __XCCDoubleCircularBuffer {
    XCCArray_s _base;
    XIndex capacity;// ==0 时 _storage = NULL; <= X_BUILD_DoubleCircularBufferPageCapacity时
    XCCDoubleCircularBufferIndex offset;
    XIndex pageCount;
    XIndex pageCapacity;
    XUInt8 * _Nullable _storage;
} XCCDoubleCircularBuffer_s;

#pragma mark - public api

void _XCCDoubleCircularBufferEnumerate(XCCArrayPtr _Nonnull array, XRange range, void * _Nullable context, XCCArrayEnumerateCallBack_f _Nonnull func);

void _XCCDoubleCircularBufferReverseEnumerate(XCCArrayPtr _Nonnull array, XRange range, void * _Nullable context, XCCArrayEnumerateCallBack_f _Nonnull func);



#endif /* XCCDoubleCircularBuffer_h */
