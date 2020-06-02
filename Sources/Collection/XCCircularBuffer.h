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

//8mb个
#define X_BUILD_CircularBufferPageCapacity X_BUILD_UInt(0x1000000)
#define X_BUILD_CircularBufferCapacityMax X_BUILD_UInt(0x8000000000000000)

typedef struct __XCCircularBufferLocation {
    XIndex page: 40;
    XIndex item: 24;
} XCCircularBufferLocation;

#else

//1mb个
#define X_BUILD_CircularBufferPageCapacity X_BUILD_UInt(0x40000)
#define X_BUILD_CircularBufferCapacityMax X_BUILD_UInt(0x80000000)

typedef struct __XCCircularBufferLocation {
    XIndex page: 14;
    XIndex item: 18;
} XCCircularBufferLocation;

#endif

static inline XIndex __XCCircularBufferGoodCapacity(XIndex capacity) {
    if (capacity >= X_BUILD_CircularBufferCapacityMax) {
        return X_BUILD_CircularBufferCapacityMax;
    } else if (capacity >= X_BUILD_CircularBufferPageCapacity) {
        return (capacity + X_BUILD_CircularBufferPageCapacity - 1) & (~(X_BUILD_CircularBufferPageCapacity - 1));
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


static inline XCCircularBufferLocation XCCircularBufferLocationMakeWithIndex(XIndex location) {
    XCCircularBufferLocation result = {};
#if CX_TARGET_RT_64_BIT
    result.item = location & X_BUILD_UInt(0xFFFFFF);
    result.page = (location >> 24);
#else
    result.item = location & X_BUILD_UInt(0x3FFFF);
    result.page = (location >> 18);
#endif
    return result;
}

static inline XIndex XCCircularBufferLocationToIndex(XCCircularBufferLocation location) {
    XIndex loc = 0;
#if CX_TARGET_RT_64_BIT
    loc += location.page;
    loc = loc << 24;
    loc += location.item;
#else
    loc += location.page;
    loc = loc << 18;
    loc += location.item;
#endif
    return loc;
}


typedef struct __XCBuffer {
    XIndex deep: 3;
    XIndex count: (XUIntBitsCount - 3);
    XPtr _Nullable items[0];
} XCBuffer_s;

typedef struct __XCCircularBuffer {
    XCArray_s _base;
    XIndex _deep;
    XIndex maxCapacity;
    XIndex _capacity;
    XIndex _offset;
    XPtr _Nullable _storage;
} XCCircularBuffer_s;

    
    
#endif /* XCCircularBuffer_h */
