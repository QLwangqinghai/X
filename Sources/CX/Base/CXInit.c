//
//  CCInit.c
//  Basic
//
//  Created by vector on 2020/2/2.
//


#include "../include/Base/XType.h"
#include "../include/Base/XAtomicInternal.h"


#pragma mark - Init

_Static_assert(sizeof(_Atomic(XFastUInt)) == sizeof(XUInt), "error");
_Static_assert(sizeof(_Atomic(XFastUInt32)) == sizeof(XUInt32), "error");
_Static_assert(sizeof(_Atomic(XFastUInt64)) == sizeof(XUInt64), "error");

__attribute__((constructor(101)))
static void __CXInit(void) {
    assert(UINT_FAST32_MAX == UINT32_MAX);
    assert(UINT_FAST64_MAX == UINT64_MAX);
    assert(sizeof(_Atomic(XFastUInt)) == sizeof(XUInt));
    assert(UINT_FAST64_MAX == UINT64_MAX);

    assert(sizeof(uintptr_t) == sizeof(XUInt));
    assert(sizeof(size_t) == sizeof(XUInt));

}
