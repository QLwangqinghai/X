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
    XIndex cid: 8;
    XIndex elementSize: (XUIntBitsCount - 8);
    XIndex count;
} XCBase_s;

#if defined(__cplusplus)
}  // extern C
#endif


#endif /* XCollection_h */
