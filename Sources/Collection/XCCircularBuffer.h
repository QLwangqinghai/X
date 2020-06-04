//
//  XCCircularBuffer.h
//  X
//
//  Created by vector on 2020/5/31.
//  Copyright © 2020 haoqi. All rights reserved.
//

#ifndef XCCircularBuffer_h
#define XCCircularBuffer_h

#include "XCollectionBase.h"

/*
 32位下 最多3维数组 9-10-12
 64位下 最多5维数组 3-13-13-13-21
 */

#if CX_TARGET_RT_64_BIT

//#define X_BUILD_CircularBufferTableCapacityShift X_BUILD_UInt(20)

//2m
#define X_BUILD_CircularBufferPageCapacity X_BUILD_UInt(0x200000)
#define X_BUILD_CircularBufferCapacityMax X_BUILD_UInt(0x8000000000000000)

typedef struct __XCCircularBufferIndex {
    XIndex page: 43;
    XIndex item: 21;
} XCCircularBufferIndex;

#else

//256k
#define X_BUILD_CircularBufferPageCapacity X_BUILD_UInt(0x40000)
#define X_BUILD_CircularBufferCapacityMax X_BUILD_UInt(0x80000000)

typedef struct __XCCircularBufferIndex {
    XIndex page: 14;
    XIndex item: 18;
} XCCircularBufferIndex;

#endif


static inline XCCircularBufferIndex XCCircularBufferIndexMake(XIndex item, XIndex page) {
    XCCircularBufferIndex result = {};
    result.item = item;
    result.page = page;
    return result;
}

static inline XCCircularBufferIndex XCCircularBufferIndexMakeWithIndex(XIndex location) {
    XCCircularBufferIndex result = {};
#if CX_TARGET_RT_64_BIT
    result.item = location & X_BUILD_UInt(0xFFFFFF);
    result.page = (location >> 21);
#else
    result.item = location & X_BUILD_UInt(0x3FFFF);
    result.page = (location >> 18);
#endif
    return result;
}

static inline XIndex XCCircularBufferIndexToIndex(XCCircularBufferIndex location) {
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

static inline XCCircularBufferIndex __XCCircularBufferAlignedCapacity(XIndex capacity) {
    XCCircularBufferIndex result = {};
    if (capacity >= X_BUILD_CircularBufferCapacityMax) {
        result.page = (X_BUILD_CircularBufferCapacityMax / X_BUILD_CircularBufferPageCapacity);
    } else if (capacity > X_BUILD_CircularBufferPageCapacity / 2) {
        result.page = (capacity + X_BUILD_CircularBufferPageCapacity - 1) / X_BUILD_CircularBufferPageCapacity;
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

static inline XBool __XCCircularBufferNeedExpansion(XCCircularBufferIndex current, XCCircularBufferIndex aligned) {
    if (aligned.page < current.page) {
        return false;
    }
    
    return false;
}

static inline XBool __XCCircularBufferNeedReduction(XCCircularBufferIndex current, XCCircularBufferIndex aligned) {

    return false;
}


static inline XIndex __XCCircularBufferPageLevelOfCapacity(XIndex capacity) {
    if (capacity == 0) {
        return 0;
    } else if (capacity <= X_BUILD_CircularBufferPageCapacity) {
        return 1;
    } else {
        return 2;
    }
}

static inline XIndex __XCCircularBufferGoodCapacity(XIndex capacity) {
    if (capacity > X_BUILD_CircularBufferPageCapacity) {
        return X_BUILD_CircularBufferCapacityMax;
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




typedef struct __XCBuffer {
    XIndex deep: 3;
    XIndex count: (XUIntBitsCount - 3);
    XPtr _Nullable items[0];
} XCBuffer_s;

typedef struct __XCCircularBuffer {
    XCArray_s _base;
    XIndex maxCapacity;
    XIndex _capacity;
    XIndex _offset;

    //capacity.page > 1 时，_storage 存储page， pageCount是个有效值，否则 _storage 存储item
    XIndex pageCapacity;
    XIndex capacity;
    XCCircularBufferIndex offset;
    XPtr _Nullable _storage;
} XCCircularBuffer_s;

    
    
#endif /* XCCircularBuffer_h */
