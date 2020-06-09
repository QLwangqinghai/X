//
//  XCCPageTable.h
//  X
//
//  Created by vector on 2020/6/9.
//  Copyright © 2020 haoqi. All rights reserved.
//

#ifndef XCCPageTable_h
#define XCCPageTable_h

#include "XCCBase.h"

typedef struct __XCCPageTable {
    XUInt capacity;
    XUInt offset;
    XUInt count;
    XPtr _Nullable cached;
    XPtr _Nullable pages[0];
} XCCPageTable_s;

static inline XIndex XCCPageTableGoodCapacity(XIndex count) {
    XIndex capacity = count + 4;
    XIndex one = 1;
    assert(count <= X_BUILD_CollectionCapacityMax);

#if CX_TARGET_RT_64_BIT
    for (XIndex i=4; i<64; i++) {
        XIndex tmp = one << i;
        if (tmp >= capacity) {
            return tmp - 4;
        }
        if (i >= 10) {
            tmp += tmp/2;
            if (tmp >= capacity) {
                return tmp - 4;
            }
        }
    }
#else
    for (XIndex i=4; i<32; i++) {
        XIndex tmp = one << i;
        if (tmp >= capacity) {
            return tmp - 4;
        }
        if (i >= 10) {
            tmp += tmp/2;
            if (tmp >= capacity) {
                return tmp - 4;
            }
        }
    }
#endif
    return X_BUILD_CollectionCapacityMax-4;
}

XCCPageTable_s * _Nonnull XCCPageTableCreate(XIndex capacity);
void XCCPageTableDestroy(XCCPageTable_s * _Nonnull table);

XCCPageTable_s * _Nullable XCCPageTableCopyByInsert(XCCPageTable_s * _Nullable table, XRange range, XIndex capacity);
XCCPageTable_s * _Nullable XCCPageTableCopyByRemove(XCCPageTable_s * _Nullable table, XRange range, XIndex capacity);

void XCCPageTableInsert(XCCPageTable_s * _Nonnull table, XRange range);
void XCCPageTableRemove(XCCPageTable_s * _Nonnull table, XRange range);

XPtr _Nullable XCCPageTableGet(XCCPageTable_s * _Nonnull table, XIndex index);
void XCCPageTableSet(XCCPageTable_s * _Nonnull table, XIndex index, XPtr _Nullable value);


#endif /* XCCPageTable_h */
