//
//  XHash.h
//  X
//
//  Created by vector on 2020/6/10.
//  Copyright © 2020 haoqi. All rights reserved.
//

#ifndef XHash_h
#define XHash_h

#include "XType.h"





extern XHashCode XHashUInt64(XUInt64 i);
extern XHashCode XHashSInt64(XSInt64 i);
extern XHashCode XHashFloat64(XFloat64 f);
extern XHashCode XHashFloat32(XFloat32 f);

extern XUInt32 _XELFHashBytes(XUInt8 * _Nullable bytes, XUInt32 length);

static inline XHashCode XAddressHash(XPtr _Nonnull obj) {
#if CX_TARGET_RT_64_BIT
    return (XHashCode)(((uintptr_t)obj) >> 3);
#else
    return (XHashCode)(((uintptr_t)obj) >> 2);
#endif
};

extern XUInt32 XMurmurHash32(const XPtr _Nonnull key, XUInt32 len, XUInt32 seed);

// 64-bit hash for 64-bit platforms
extern XUInt64 XMurmurHash64(const XPtr _Nonnull key, XUInt len, XUInt32 seed);



typedef struct {
    uint64_t v[4];
} XSipHashState;

extern void XSipHashStateInit(XSipHashState * _Nonnull state, XUInt64 key[_Nonnull 2]);


#endif /* XHash_h */
