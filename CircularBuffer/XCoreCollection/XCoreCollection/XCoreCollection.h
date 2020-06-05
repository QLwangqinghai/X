//
//  XCoreCollection.h
//  XCoreCollection
//
//  Created by vector on 2020/6/5.
//  Copyright © 2020 haoqi. All rights reserved.
//

#ifndef XCoreCollection_h
#define XCoreCollection_h

#include "XType.h"


#pragma mark - 环形缓冲区

/**
 * 把range中的元素前移n
 * 前置条件: range.location + range.length <= capacity,   range.location >= n,
 */
void XCCCircularBufferMoveForward(XUInt8 * _Nonnull buffer, XIndex capacity, XIndex offset, XIndex elementSize, XRange range, XIndex n);

/**
* 把range中的元素后移n
* 前置条件: range.location + range.length <= capacity,  capacity - (range.location + range.length) >= n
*/
void XCCCircularBufferMoveBackward(XUInt8 * _Nonnull buffer, XIndex capacity, XIndex offset, XIndex elementSize, XRange range, XIndex n);

/**
* 把range中的元素清零
* 前置条件: range.location + range.length <= capacity
*/
void XCCCircularBufferBezero(XUInt8 * _Nonnull buffer, XIndex capacity, XIndex offset, XIndex elementSize, XRange range);


#endif /* XCoreCollection_h */
