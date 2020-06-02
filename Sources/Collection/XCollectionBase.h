//
//  XCollectionBase.h
//  X
//
//  Created by clf on 2020/6/1.
//  Copyright © 2020 haoqi. All rights reserved.
//

#ifndef XCollectionBase_h
#define XCollectionBase_h

#if defined(__cplusplus)
extern "C" {
#endif

#include "XAtomic.h"

typedef struct __XCBase {
    _Atomic(XFastUInt) counter;
    XIndex cid: 10;
    XIndex elementSize: (XUIntBitsCount - 10);
    XIndex count;
} XCBase_s;



typedef struct __XCArray {
    _Atomic(XFastUInt) counter;
    XIndex cid: 10;
    XIndex elementSize: (XUIntBitsCount - 10);
    XIndex count;
} XCArray_s;

typedef XPtr XCollectionPtr;

typedef XCollectionPtr XCArrayPtr;

XIndex XCArrayGetCount(XCArrayPtr _Nonnull array);
XIndex XCArrayGetElementSize(XCArrayPtr _Nonnull array);


typedef void (*XCArrayEnumerateCallBack_f)(XPtr _Nullable context, XRange range, size_t elementSize, const void * _Nonnull values, XBool * _Nonnull stop);

void XCArrayEnumerate(XCArrayPtr _Nonnull array, XRange range, void * _Nullable context, XCArrayEnumerateCallBack_f _Nonnull func);






#if defined(__cplusplus)
}  // extern C
#endif


#endif /* XCollection_h */
