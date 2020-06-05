//
//  main.c
//  XCoreCollection
//
//  Created by vector on 2020/6/5.
//  Copyright © 2020 haoqi. All rights reserved.
//

#include <stdio.h>
#include <string.h>

#define false 0
#define true 1
typedef void * XPtr;

typedef uint32_t XIndex;

typedef uint32_t XBool;
typedef uint8_t XUInt8;

typedef struct {
    XIndex location;
    XIndex length;
} XRange;

#define XAssert(cond, func, desc) {\
    if (!(cond)) {\
        fprintf(stderr, "%s error, %s", func, desc);\
        abort();\
    }\
}

//6789前移2
//789000000123456 => 900000000123678
//1 3, 3 1    1 2 1
//2 3, 3 2    2 1 2
//2 3, 4 1    2 2 1
//5 2, 6 1    5 1 1


//3 1, 1 3    1 2 1
//2 3, 3 2    2 1 2
//2 3, 4 1    2 2 1
//5 2, 6 1    5 1 1

//isBackward == true 时， 先移动后半部分
static inline XIndex __XCCCircularBufferSplitCounts(XIndex capacity, XIndex begin, XIndex dstBegin, XIndex length, XIndex * _Nonnull mcounts, XBool isBackward) {
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
void XCCCircularBufferMoveForward(XUInt8 * _Nonnull buffer, XIndex capacity, XIndex offset, XIndex elementSize, XRange range, XIndex n, XBool isBackward) {
    XIndex begin = (range.location + offset) % capacity;
    XIndex dstBegin = (range.location - n + offset) % capacity;
    XIndex mcounts[3] = {};
    XIndex sliceCount = __XCCCircularBufferSplitCounts(capacity, begin, dstBegin, range.length, mcounts, false);
    
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
    XIndex sliceCount = __XCCCircularBufferSplitCounts(capacity, begin, dstBegin, range.length, mcounts, true);

    XIndex sIndex = begin + range.length;
    XIndex dIndex = dstBegin + range.length;
    for (XIndex i=0; i<sliceCount; i++) {
        XIndex mcount = mcounts[i];
        sIndex -= mcount;
        dIndex -= mcount;
        memmove(buffer + (dIndex % capacity) * elementSize, buffer + (sIndex % capacity) * elementSize, mcount * elementSize);
    }
}



int main(int argc, const char * argv[]) {
    // insert code here...
    printf("Hello, World!\n");
    return 0;
}
