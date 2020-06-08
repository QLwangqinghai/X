//
//  XCCBuffer.h
//  X
//
//  Created by vector on 2020/6/8.
//  Copyright © 2020 haoqi. All rights reserved.
//

#ifndef XCCBuffer_h
#define XCCBuffer_h

#include "XCCBase.h"

typedef struct __XCCBuffer {
    XCCArray_s _base;
    XUInt8 _storage[0];
} XCCBuffer_s;

XPtr _Nonnull XCCBufferGetValues(XCCBuffer_s * _Nonnull buffer, XRange range);
void XCCBufferSetValues(XCCBuffer_s * _Nonnull buffer, XRange range, XPtr _Nonnull values);


void XCCBufferSetValue(XCCBuffer_s * _Nonnull buffer, XIndex location, XPtr _Nonnull values);
XPtr _Nonnull XCCBufferGetValue(XCCBuffer_s * _Nonnull buffer, XIndex location);

#endif /* XCCBuffer_h */
