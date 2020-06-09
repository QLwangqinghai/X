//
//  XCCBase.c
//  X
//
//  Created by clf on 2020/6/1.
//  Copyright © 2020 haoqi. All rights reserved.
//

#include "XCCBase.h"
#include "XMemory.h"


XUInt8 * _Nonnull XCCAllocate(XSize elementSize, XIndex capacity) {
    XUInt8 * ptr = NULL;
    if (capacity % 64 == 0) {
        ptr = XAlignedAllocate(elementSize * capacity, 64);
    } else {
        ptr = XAllocate(elementSize * capacity);
    }
    assert(ptr);
    memset(ptr, 0, elementSize * capacity);
    return ptr;
}
