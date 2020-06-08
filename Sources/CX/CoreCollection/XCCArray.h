//
//  XCCArray.h
//  X
//
//  Created by vector on 2020/6/8.
//  Copyright © 2020 haoqi. All rights reserved.
//

#ifndef XCCArray_h
#define XCCArray_h

#include "XCCBase.h"


XIndex XCArrayGetCount(XCCArrayPtr _Nonnull array);
XIndex XCArrayGetElementSize(XCCArrayPtr _Nonnull array);
void XCArrayEnumerate(XCCArrayPtr _Nonnull array, XRange range, void * _Nullable context, XCCArrayEnumerateCallBack_f _Nonnull func);
void XCArrayReverseEnumerate(XCCArrayPtr _Nonnull array, XRange range, void * _Nullable context, XCCArrayEnumerateCallBack_f _Nonnull func);


#endif /* XCCArray_h */
