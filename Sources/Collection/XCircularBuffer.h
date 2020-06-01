//
//  XCircularBuffer.h
//  X
//
//  Created by vector on 2020/5/31.
//  Copyright © 2020 haoqi. All rights reserved.
//

#ifndef XCircularBuffer_h
#define XCircularBuffer_h

#include "XType.h"
#include "XAtomic.h"

/*
 32位下 最多2维数组 3-8-20 Tables-Pages-Items
 64位下 最多3维数组 3-20-20-20 Pool-Tables-Pages-Items
 
 */

#if CX_TARGET_RT_64_BIT
#define X_BUILD_CircularBufferTableListCapacity X_BUILD_UInt(0x100000)
#define X_BUILD_CircularBufferTableCapacity X_BUILD_UInt(0x100000)
//#define X_BUILD_CircularBufferTableCapacityShift X_BUILD_UInt(20)

//8mb个
#define X_BUILD_CircularBufferPageCapacity X_BUILD_UInt(0x800000)
#define X_BUILD_CircularBufferCapacityMax X_BUILD_UInt(0x8000000000000000)

typedef struct __XCircularBufferLocation {
    XIndex table: 21;
    XIndex page: 20;
    XIndex item: 23;
} XCircularBufferLocation;

#else

#define X_BUILD_CircularBufferTableCapacity X_BUILD_UInt(0x800)

//1mb个
#define X_BUILD_CircularBufferPageCapacity X_BUILD_UInt(0x100000)

#define X_BUILD_CircularBufferCapacityMax X_BUILD_UInt(0x80000000)

typedef struct __XCircularBufferLocation {
    XIndex page: 12;
    XIndex item: 20;
} XCircularBufferLocation;

#endif

static inline XIndex __CCCircularBufferGoodCapacity(XIndex capacity) {
    
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


static inline XCircularBufferLocation XCircularBufferLocationMakeWithIndex(XIndex location) {
    XCircularBufferLocation result = {};
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

static inline XIndex XCircularBufferLocationToIndex(XCircularBufferLocation location) {
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


typedef struct __XCircularBufferPage {
    XUInt8 items[sizeof(XUInt)];
} XCircularBufferPage_s;

typedef struct __XCircularBufferPageTable {
    XCircularBufferPage_s * _Nullable pages[X_BUILD_CircularBufferTableCapacity];
} XCircularBufferPageTable_s;

#if CX_TARGET_RT_64_BIT
typedef struct __XCircularBufferPageTableList {
    XCircularBufferPageTable_s * _Nullable tables[X_BUILD_CircularBufferTableCapacity];
} XCircularBufferPageTableList_s;
#endif

//_storage 采用多维数组，减少内存抖动

typedef struct __XCircularBuffer {
    _Atomic(XFastUInt) _counter;
    XIndex id;
    XIndex _elementSize;

    XIndex _pageSize;
    XIndex _pageTableSize;
    XIndex _tableListSize;
    
    XIndex _deep;
    XIndex _capacity;
    XIndex _offset;
    XIndex _count;
    XPtr _Nullable _storage[8];
} XCircularBuffer_s;

#endif /* XCircularBuffer_h */
