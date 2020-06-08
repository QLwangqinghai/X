//
//  XCCCircularBuffer.h
//  X
//
//  Created by vector on 2020/6/8.
//  Copyright © 2020 haoqi. All rights reserved.
//

#ifndef XCCCircularBuffer_h
#define XCCCircularBuffer_h

#include "XCCBase.h"

typedef struct __XCCCircularBuffer {
    XCCArray_s _base;
    XIndex capacity;
    XIndex offset;
    XUInt8 * _Nullable _storage;
} XCCCircularBuffer_s;

void XCCCircularBufferInsertResize(XCCCircularBuffer_s * _Nonnull buffer, XRange range, XIndex capacity);
void XCCCircularBufferRemoveResize(XCCCircularBuffer_s * _Nonnull buffer, XRange range, XIndex capacity);

#endif /* XCCCircularBuffer_h */
