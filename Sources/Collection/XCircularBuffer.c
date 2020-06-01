//
//  XCircularBuffer.c
//  X
//
//  Created by vector on 2020/5/31.
//  Copyright © 2020 haoqi. All rights reserved.
//

#include "XCircularBuffer.h"
#include "XMemory.h"

static inline XCircularBufferPage_s * _Nonnull _XCircularBufferPageCreate(XIndex count) {
    return XAlignedAllocate(sizeof(XCircularBufferPage_s), 64);
}

static inline XCircularBufferPageTable_s * _Nonnull _XCircularBufferPageTableCreate(void) {
    return XAlignedAllocate(sizeof(XCircularBufferPageTable_s), 64);
}

#if CX_TARGET_RT_64_BIT
static inline XCircularBufferPageTableList_s * _Nonnull _XCircularBufferPageTableListCreate(void) {
    return XAlignedAllocate(sizeof(XCircularBufferPageTableList_s), 64);
}
#endif



static inline XCircularBufferLocation __XCircularBufferLoactionOfIndex(XCircularBuffer_s * _Nonnull buffer, XIndex index) {
    XIndex r = buffer->_capacity - buffer->_offset;
    XIndex location = 0;
    if (index >= r) {
        location = index - r;
    } else {
        location = index + buffer->_offset;
    }
    return XCircularBufferLocationMakeWithIndex(location);
}

static inline XIndex __XCircularBufferIndexOfLoaction(XCircularBuffer_s * _Nonnull buffer, XCircularBufferLocation location) {
    XIndex loc = XCircularBufferLocationToIndex(location);
    if (loc >= buffer->_offset) {
        return loc - buffer->_offset;
    } else {
        return buffer->_capacity - (buffer->_offset - loc);
    }
}
