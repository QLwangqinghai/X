//
//  XCCBase.h
//  X
//
//  Created by clf on 2020/6/1.
//  Copyright © 2020 haoqi. All rights reserved.
//

#ifndef XCCBase_h
#define XCCBase_h

#if defined(__cplusplus)
extern "C" {
#endif

#include "XAtomic.h"


#if CX_TARGET_RT_64_BIT

#define X_BUILD_CollectionCapacityMax X_BUILD_UInt(0x8000000000000000)

#else

#define X_BUILD_CollectionCapacityMax X_BUILD_UInt(0x80000000)

#endif





typedef struct __XCCBase {
    XIndex cid: 10;
    XIndex elementSize: (XUIntBitsCount - 10);
    XIndex count;
} XCCBase_s;

static inline void XCCBaseInit(XCCBase_s * _Nonnull base, XIndex cid, XIndex elementSize, XIndex count) {
    
}


typedef struct __XCCArraySlice {
    XRange range;
    XIndex elementSize;
    XPtr _Nonnull values;
} XCCArraySlice;


typedef XCCBase_s XCCArray_s;

typedef XPtr XCCCollectionPtr;

typedef XCCCollectionPtr XCCArrayPtr;

typedef void (*XCCArrayEnumerateCallBack_f)(XPtr _Nullable context, XCCArraySlice slice, XBool * _Nonnull stop);




XUInt8 * _Nonnull XCCAllocate(XSize elementSize, XIndex capacity);



#if defined(__cplusplus)
}  // extern C
#endif


#endif /* XCCBase_h */
