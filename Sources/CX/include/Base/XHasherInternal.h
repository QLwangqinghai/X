//
//  XHasher.h
//  X
//
//  Created by vector on 2020/6/12.
//  Copyright © 2020 haoqi. All rights reserved.
//

#ifndef XHasher_h
#define XHasher_h

#include "XType.h"
#include "XHash.h"

typedef void (*XHasherCombineUInt64_f)(XHasher _Nonnull hasher, XUInt64 value);
typedef void (*XHasherCombineUInt32_f)(XHasher _Nonnull hasher, XUInt32 value);
typedef void (*XHasherCombineBytes_f)(XHasher _Nonnull hasher, XUInt8 * _Nullable buffer, XSize size);

typedef struct __XBaseHasher {
    XHasherCombineUInt64_f _Nonnull combineUInt64;
    XHasherCombineUInt32_f _Nonnull combineUInt32;
    XHasherCombineBytes_f _Nonnull combineBytes;
} XBaseHasher_t;

typedef struct __XSipHasher {
    XBaseHasher_t base;
    XSipHashContext context;
} XSipHasher;

static inline XUInt64 XSipHasherFinalize(XSipHasher * _Nonnull hasher) {
    return XSipHashFinalize(&(hasher->context));
}


#endif /* XHasher_h */
