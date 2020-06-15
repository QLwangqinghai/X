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

typedef struct {
    XSipHashState state;
    XUInt64 length;
    union {
      uint8_t bytes[8];
      uint64_t word;
    } pendingBlock;
} XSipHashContext;

extern void XSipHashStateInit(XSipHashState * _Nonnull state, XUInt64 key[_Nonnull 2]);

extern XUInt64 XSipHash(const XSipHashState * _Nonnull state, const XUInt8 * _Nonnull bytes, XUInt64 length);


extern void XSipHashInit(XSipHashContext * _Nonnull context, XUInt64 key[_Nonnull 2]);
extern void XSipHashInitWithState(XSipHashContext * _Nonnull context, const XSipHashState * _Nonnull state);

extern void XSipHashCompress(XSipHashContext * _Nonnull context, const XUInt8 * _Nonnull bytes, XUInt64 length);
extern XUInt64 XSipHashFinalize(XSipHashContext * _Nonnull context);

#endif /* XHash_h */
