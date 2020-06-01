//
//  XCCircularBuffer.c
//  X
//
//  Created by vector on 2020/5/31.
//  Copyright © 2020 haoqi. All rights reserved.
//

#include "XCCircularBuffer.h"
#include "XMemory.h"

static inline XCCircularBufferPage_s * _Nonnull _XCCircularBufferPageCreate(XIndex count) {
    return XAlignedAllocate(sizeof(XCCircularBufferPage_s), 64);
}

static inline XCCircularBufferPageTable_s * _Nonnull _XCCircularBufferPageTableCreate(void) {
    return XAlignedAllocate(sizeof(XCCircularBufferPageTable_s), 64);
}

#if CX_TARGET_RT_64_BIT
static inline XCCircularBufferPageTableList_s * _Nonnull _XCCircularBufferPageTableListCreate(void) {
    return XAlignedAllocate(sizeof(XCCircularBufferPageTableList_s), 64);
}
#endif



static inline XCCircularBufferLocation __XCCircularBufferLoactionOfIndex(XCCircularBuffer_s * _Nonnull buffer, XIndex index) {
    XIndex r = buffer->_capacity - buffer->_offset;
    XIndex location = 0;
    if (index >= r) {
        location = index - r;
    } else {
        location = index + buffer->_offset;
    }
    return XCCircularBufferLocationMakeWithIndex(location);
}

static inline XIndex __XCCircularBufferIndexOfLoaction(XCCircularBuffer_s * _Nonnull buffer, XCCircularBufferLocation location) {
    XIndex loc = XCCircularBufferLocationToIndex(location);
    if (loc >= buffer->_offset) {
        return loc - buffer->_offset;
    } else {
        return buffer->_capacity - (buffer->_offset - loc);
    }
}
