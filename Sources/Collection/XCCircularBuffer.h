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
 32位下 最多2维数组 13-18 Pages-Items
 64位下 最多3维数组 23-20-20 Pool-Tables-Pages-Items
 */

#if CX_TARGET_RT_64_BIT
#define X_BUILD_CircularBufferTableListCapacity X_BUILD_UInt(0x100000)
#define X_BUILD_CircularBufferTableCapacity X_BUILD_UInt(0x100000)
//#define X_BUILD_CircularBufferTableCapacityShift X_BUILD_UInt(20)

//8mb个
#define X_BUILD_CircularBufferPageCapacity X_BUILD_UInt(0x800000)
#define X_BUILD_CircularBufferCapacityMax X_BUILD_UInt(0x8000000000000000)

typedef struct __XCCircularBufferLocation {
    XIndex pool: 3;
    XIndex table: 21;
    XIndex page: 20;
    XIndex item: 23;
} XCCircularBufferLocation;

#else

#define X_BUILD_CircularBufferTableCapacity X_BUILD_UInt(0x400)
//1mb个
#define X_BUILD_CircularBufferPageCapacity X_BUILD_UInt(0x40000)
#define X_BUILD_CircularBufferCapacityMax X_BUILD_UInt(0x80000000)

typedef struct __XCCircularBufferLocation {
    XIndex page: 12;
    XIndex item: 20;
} XCCircularBufferLocation;

#endif

static inline XIndex __XCCircularBufferGoodCapacity(XIndex capacity) {
    
#if CX_TARGET_RT_64_BIT
    if (capacity > X_BUILD_CircularBufferPageCapacity * X_BUILD_CircularBufferTableCapacity) {
        return X_BUILD_CircularBufferCapacityMax;
    } else if (capacity > X_BUILD_CircularBufferPageCapacity) {
        return X_BUILD_CircularBufferPageCapacity * X_BUILD_CircularBufferTableCapacity;
    } else {
#else
    if (capacity > X_BUILD_CircularBufferPageCapacity) {
        return X_BUILD_CircularBufferCapacityMax;
    } else {
#endif
        
        if (capacity <= 0) {
            return 0;
        } else if (capacity == X_BUILD_CircularBufferPageCapacity) {
            return X_BUILD_CircularBufferPageCapacity;
        } else {
            return 0;
        }
    }
}


static inline XCCircularBufferLocation XCCircularBufferLocationMakeWithIndex(XIndex location) {
    XCCircularBufferLocation result = {};
#if CX_TARGET_RT_64_BIT
    result.item = location & X_BUILD_UInt(0x7FFFFF);
    result.page = (location >> 23) & X_BUILD_UInt(0x7FFFFF);
    result.table = (location >> 43);
#else
    result.item = location & X_BUILD_UInt(0xFFFFF);
    result.page = (location >> 20);
#endif
    return result;
}

static inline XIndex XCCircularBufferLocationToIndex(XCCircularBufferLocation location) {
    XIndex loc = 0;
#if CX_TARGET_RT_64_BIT
    loc += location.table;
    loc = loc << 20;
    loc += location.page;
    loc = loc << 23;
    loc += location.item;
#else
    loc += location.page;
    loc = loc << 20;
    loc += location.item;
#endif
    return loc;
}


typedef struct __XCCircularBufferPage {
    XUInt8 items[sizeof(XUInt)];
} XCCircularBufferPage_s;

typedef struct __XCCircularBufferPageTable {
    XCCircularBufferPage_s * _Nullable pages[X_BUILD_CircularBufferTableCapacity];
} XCCircularBufferPageTable_s;

#if CX_TARGET_RT_64_BIT
typedef struct __XCCircularBufferPageTableList {
    XCCircularBufferPageTable_s * _Nullable tables[X_BUILD_CircularBufferTableCapacity];
} XCCircularBufferPageTableList_s;
#endif

//_storage 采用多维数组，减少内存抖动

typedef struct __XCBuffer {
    XIndex deep: 3;
    XIndex count: (XUIntBitsCount - 3);
    XPtr _Nullable items[0];
} XCBuffer_s;

    
typedef struct __XCCircularBuffer {
    XCBase_s _base;
    XIndex maxCapacity;
    XIndex _capacity;
    XIndex _offset;
    XPtr _Nullable _storage;
} XCCircularBuffer_s;

#endif /* XCCircularBuffer_h */
