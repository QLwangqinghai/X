//
//  XCCUtil.h
//  X
//
//  Created by vector on 2020/6/8.
//  Copyright © 2020 haoqi. All rights reserved.
//

#ifndef XCCUtil_h
#define XCCUtil_h

#if defined(__cplusplus)
extern "C" {
#endif

#include "XType.h"


#pragma mark - 环形缓冲区

/**
 * 把range中的元素前移n
 * 前置条件: range.location + range.length <= capacity,   range.location >= n,
 */
void XCCUtilCircularBufferMoveForward(XUInt8 * _Nonnull buffer, XIndex capacity, XIndex offset, XIndex elementSize, XRange range, XIndex n);

/**
* 把range中的元素后移n
* 前置条件: range.location + range.length <= capacity,  capacity - (range.location + range.length) >= n
*/
void XCCUtilCircularBufferMoveBackward(XUInt8 * _Nonnull buffer, XIndex capacity, XIndex offset, XIndex elementSize, XRange range, XIndex n);

/**
* 把range中的元素清零
* 前置条件: range.location + range.length <= capacity
*/
void XCCUtilCircularBufferBezero(XUInt8 * _Nonnull buffer, XIndex capacity, XIndex offset, XIndex elementSize, XRange range);


/**
* 把range中的元素后移n
* 前置条件: dstBuffer!=buffer, range.location + range.length <= capacity,  capacity - (range.location + range.length) >= n
*/
void XCCUtilCircularBufferCopy(XUInt8 * _Nonnull dstBuffer, XIndex dstCapacity, XIndex dstOffset, XIndex dstIndex, XUInt8 * _Nonnull buffer, XIndex capacity, XIndex offset, XIndex index, XIndex length, XIndex elementSize);




#if defined(__cplusplus)
}  // extern C
#endif


#endif /* XCCUtil_h */
