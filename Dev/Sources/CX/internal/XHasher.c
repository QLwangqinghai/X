//
//  XHasher.c
//  X
//
//  Created by vector on 2020/6/12.
//  Copyright © 2020 haoqi. All rights reserved.
//

#include "XHasher.h"

void XSipHasherCombineUInt64(XHasher _Nonnull hasher, XUInt64 value) {
    XSipHasher * h = (XSipHasher *)hasher;
    XSipHashCompress(&(h->context), (const XUInt8 *)(&value), sizeof(XUInt64));
    
}
void XSipHasherCombineUInt32(XHasher _Nonnull hasher, XUInt32 value) {
    XSipHasher * h = (XSipHasher *)hasher;
    XSipHashCompress(&(h->context), (const XUInt8 *)(&value), sizeof(XUInt32));
}

void XSipHasherCombineBytes(XHasher _Nonnull hasher, XUInt8 * _Nullable buffer, XSize size) {
    XSipHasher * h = (XSipHasher *)hasher;
    XSipHashCompress(&(h->context), buffer, size);
}


