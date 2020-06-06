//
//  XCoreCollection.c
//  XCoreCollection
//
//  Created by vector on 2020/6/5.
//  Copyright © 2020 haoqi. All rights reserved.
//

#include "XCoreCollection.h"

//isBackward == true 时， 先移动后半部分
static inline XIndex __XCCCircularBufferSplitMoveCounts(XIndex capacity, XIndex begin, XIndex dstBegin, XIndex length, XIndex * _Nonnull mcounts, XBool isBackward) {
    XIndex p = capacity - length;
    XIndex counts[2] = {};
    if (begin > p) {
        if (isBackward) {
            counts[1] = capacity - begin;
            counts[0] = length - counts[1];
        } else {
            counts[0] = capacity - begin;
            counts[1] = length - counts[0];
        }
    } else {
        counts[0] = length;
        counts[1] = 0;
    }
    
    XIndex dstCounts[2] = {};
    if (dstBegin > p) {
        if (isBackward) {
            dstCounts[1] = capacity - dstBegin;
            dstCounts[0] = length - dstCounts[1];
        } else {
            dstCounts[0] = capacity - dstBegin;
            dstCounts[1] = length - dstCounts[0];
        }
    } else {
        dstCounts[0] = length;
        dstCounts[1] = 0;
    }
    
    //最多移动3次
    int mIndex = 0;
    int cIndex = 0;
    int dcIndex = 0;

    XIndex remain = length;
    while (remain > 0) {
        if (counts[cIndex] > dstCounts[dcIndex]) {
            mcounts[mIndex] = dstCounts[dcIndex];
            counts[dcIndex] -= mcounts[mIndex];
            dstCounts[dcIndex] -= mcounts[mIndex];
            dcIndex += 1;
        } else if (counts[cIndex] < dstCounts[dcIndex]) {
            mcounts[mIndex] = counts[dcIndex];
            counts[dcIndex] -= mcounts[mIndex];
            dstCounts[dcIndex] -= mcounts[mIndex];
            cIndex += 1;
        } else {
            mcounts[mIndex] = dstCounts[dcIndex];
            counts[dcIndex] -= mcounts[mIndex];
            dstCounts[dcIndex] -= mcounts[mIndex];
            dcIndex += 1;
            cIndex += 1;
        }
        remain -= mcounts[mIndex];
        mIndex += 1;
    }
    return mIndex;
}


/**
 * 把range中的元素前移n
 * 前置条件: range.location + range.length <= capacity,   range.location >= n,
 */
void XCCCircularBufferMoveForward(XUInt8 * _Nonnull buffer, XIndex capacity, XIndex offset, XIndex elementSize, XRange range, XIndex n) {
    XIndex begin = (range.location + offset) % capacity;
    XIndex dstBegin = (range.location - n + offset) % capacity;
    XIndex mcounts[3] = {};
    XIndex sliceCount = __XCCCircularBufferSplitMoveCounts(capacity, begin, dstBegin, range.length, mcounts, false);
    
    XIndex sIndex = begin;
    XIndex dIndex = dstBegin;
    for (XIndex i=0; i<sliceCount; i++) {
        XIndex mcount = mcounts[i];
        memmove(buffer + (dIndex % capacity) * elementSize, buffer + (sIndex % capacity) * elementSize, mcount * elementSize);
        sIndex += mcount;
        dIndex += mcount;
    }
}

/**
* 把range中的元素后移n
* 前置条件: range.location + range.length <= capacity,  capacity - (range.location + range.length) >= n
*/
void XCCCircularBufferMoveBackward(XUInt8 * _Nonnull buffer, XIndex capacity, XIndex offset, XIndex elementSize, XRange range, XIndex n) {
    XIndex begin = (range.location + offset) % capacity;
    XIndex dstBegin = (range.location + n + offset) % capacity;
    XIndex mcounts[3] = {};
    XIndex sliceCount = __XCCCircularBufferSplitMoveCounts(capacity, begin, dstBegin, range.length, mcounts, true);

    XIndex sIndex = begin + range.length;
    XIndex dIndex = dstBegin + range.length;
    for (XIndex i=0; i<sliceCount; i++) {
        XIndex mcount = mcounts[i];
        sIndex -= mcount;
        dIndex -= mcount;
        memmove(buffer + (dIndex % capacity) * elementSize, buffer + (sIndex % capacity) * elementSize, mcount * elementSize);
    }
}


/**
* 把range中的元素后移n
* 前置条件: range.location + range.length <= capacity,  capacity - (range.location + range.length) >= n
*/
void XCCCircularBufferBezero(XUInt8 * _Nonnull buffer, XIndex capacity, XIndex offset, XIndex elementSize, XRange range) {
    XIndex begin = (range.location + offset) % capacity;
    XIndex p = capacity - range.length;
    if (begin > p) {
        XIndex count = capacity - begin;
        memset(buffer + begin * elementSize, 0, count * elementSize);
        memset(buffer, 0, (range.length - count) * elementSize);
    } else {
        memset(buffer + begin * elementSize, 0, range.length * elementSize);
    }
    
}



//isBackward == true 时， 先移动后半部分
static inline XIndex __XCCCircularBufferSplitCopyCounts(XIndex capacity, XIndex begin, XIndex dstCapacity, XIndex dstBegin, XIndex length, XIndex * _Nonnull mcounts) {
    XIndex p = capacity - length;
    XIndex counts[2] = {};
    if (begin > p) {
        counts[0] = capacity - begin;
        counts[1] = length - counts[0];
    } else {
        counts[0] = length;
        counts[1] = 0;
    }
    
    XIndex dp = dstCapacity - length;
    XIndex dstCounts[2] = {};
    if (dstBegin > dp) {
        dstCounts[0] = dstCapacity - dstBegin;
        dstCounts[1] = length - dstCounts[0];
    } else {
        dstCounts[0] = length;
        dstCounts[1] = 0;
    }
    
    //最多移动3次
    int mIndex = 0;
    int cIndex = 0;
    int dcIndex = 0;

    XIndex remain = length;
    while (remain > 0) {
        if (counts[cIndex] > dstCounts[dcIndex]) {
            mcounts[mIndex] = dstCounts[dcIndex];
            counts[dcIndex] -= mcounts[mIndex];
            dstCounts[dcIndex] -= mcounts[mIndex];
            dcIndex += 1;
        } else if (counts[cIndex] < dstCounts[dcIndex]) {
            mcounts[mIndex] = counts[dcIndex];
            counts[dcIndex] -= mcounts[mIndex];
            dstCounts[dcIndex] -= mcounts[mIndex];
            cIndex += 1;
        } else {
            mcounts[mIndex] = dstCounts[dcIndex];
            counts[dcIndex] -= mcounts[mIndex];
            dstCounts[dcIndex] -= mcounts[mIndex];
            dcIndex += 1;
            cIndex += 1;
        }
        remain -= mcounts[mIndex];
        mIndex += 1;
    }
    return mIndex;
}



void XCCCircularBufferCopy(XUInt8 * _Nonnull dstBuffer, XIndex dstCapacity, XIndex dstOffset, XIndex dstIndex, XUInt8 * _Nonnull buffer, XIndex capacity, XIndex offset, XIndex index, XIndex length, XIndex elementSize) {
    
    XIndex begin = (index + offset) % capacity;
    XIndex dstBegin = (dstIndex + dstOffset) % dstCapacity;
    XIndex ccounts[3] = {};
    XIndex sliceCount = __XCCCircularBufferSplitCopyCounts(capacity, begin, dstCapacity, dstBegin, length, ccounts);
    
    XIndex sIndex = begin;
    XIndex dIndex = dstBegin;
    for (XIndex i=0; i<sliceCount; i++) {
        XIndex mcount = ccounts[i];
        memmove(dstBuffer + (dIndex % dstCapacity) * elementSize, buffer + (sIndex % capacity) * elementSize, mcount * elementSize);
        sIndex += mcount;
        dIndex += mcount;
    }
    
}
