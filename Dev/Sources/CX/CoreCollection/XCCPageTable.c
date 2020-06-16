//
//  XCCPageTable.c
//  X
//
//  Created by vector on 2020/6/9.
//  Copyright © 2020 haoqi. All rights reserved.
//

#include "XCCPageTable.h"

#include "XMemory.h"
#include "XCCUtil.h"



XCCPageTable_s * _Nonnull XCCPageTableCreate(XIndex capacity) {
    XCCPageTable_s * table = (XCCPageTable_s *)XCCAllocate(sizeof(XPtr), capacity);
    return table;
}

void XCCPageTableDestroy(XCCPageTable_s * _Nonnull table) {
    if (table) {
        free(table);
    }
}
static inline void __XCCPageTableEnumerate(XCCPageTable_s * _Nonnull table, XRange range, void * _Nullable context, XCCArrayEnumerateCallBack_f _Nonnull func) {
    
    
}
static inline void __XCCPageTableReverseEnumerate(XCCPageTable_s * _Nonnull table, XRange range, void * _Nullable context, XCCArrayEnumerateCallBack_f _Nonnull func) {
    
    
}

static inline void __XCCPageTableCopy(XCCPageTable_s * _Nonnull to, XIndex toIndex, XCCPageTable_s * _Nonnull from, XIndex fromIndex, XIndex length) {
    if (length > 0) {
        XCCUtilCircularBufferCopy((XUInt8 *)&(to->pages[0]), to->capacity, to->offset, toIndex, (XUInt8 *)&(from->pages[0]), from->capacity, from->offset, fromIndex, length, sizeof(XPtr));
    }
}

/**
 * 把range中的元素前移n
 * 前置条件: range.location + range.length <= capacity,   range.location >= n,
 */
void __XCCPageTableMoveForward(XCCPageTable_s * _Nonnull table, XRange range, XIndex n) {
    if (range.length <= 0) {
        return;
    }
    
    XCCUtilCircularBufferMoveForward((XUInt8 *)&(table->pages[0]), table->capacity, table->offset, sizeof(XPtr), range, n);
}

/**
* 把range中的元素后移n
* 前置条件: range.location + range.length <= capacity,  capacity - (range.location + range.length) >= n
*/
void __XCCPageTableMoveBackward(XCCPageTable_s * _Nonnull table, XRange range, XIndex n) {
    if (range.length <= 0) {
        return;
    }
    XCCUtilCircularBufferMoveBackward((XUInt8 *)&(table->pages[0]), table->capacity, table->offset, sizeof(XPtr), range, n);
}



//在location位置， 插入length

//0 < length
//0 <= location <= table->_base.count
static inline void __XCCPageTableInsert(XCCPageTable_s * _Nonnull table, XRange range) {
    XIndex location = range.location;
    XIndex length = range.length;
    XIndex newCount = table->count + length;

    //移动
    if (location < table->count - location) {
        //移动前半部分
        __XCCPageTableMoveForward(table, XRangeMake(0, location), length);
        table->count = newCount;
        table->offset = (table->offset + table->capacity - length) % table->capacity;
    } else {
        //移动后半部分
        __XCCPageTableMoveBackward(table, XRangeMake(location+length, table->count - location), length);
        table->count = newCount;
    }
    
}


//从location开始删除length个

//0 < length <= table->_base.count
//0 < length + location <= table->_base.count
static inline void __XCCPageTableRemove(XCCPageTable_s * _Nonnull table, XRange range) {
    XIndex location = range.location;
    XIndex length = range.length;
    XIndex newCount = table->count - length;

    //移动
    if (location < table->count - location - length) {
        //移动前半部分
        __XCCPageTableMoveBackward(table, XRangeMake(0, location), length);
        table->count = newCount;
        table->offset = (table->offset + length) % table->capacity;
    } else {
        //移动后半部分
        __XCCPageTableMoveForward(table, XRangeMake(location+length, table->count - location), length);
        table->count = newCount;
    }
}


XCCPageTable_s * _Nullable XCCPageTableCopyByInsert(XCCPageTable_s * _Nullable table, XRange range, XIndex capacity) {
    XIndex count = 0;
    if (table) {
        count = table->count;
    }
    XAssert(NULL != table, __func__, "");
    XAssert(range.location < X_BUILD_CollectionCapacityMax, __func__, "");
    XAssert(range.length <= X_BUILD_CollectionCapacityMax, __func__, "");
    XAssert(count + range.length <= X_BUILD_CollectionCapacityMax, __func__, "");
    XAssert(range.location <= count, __func__, "");
    XAssert(capacity <= X_BUILD_CollectionCapacityMax, __func__, "");
    XAssert(capacity >= count + range.length, __func__, "");
    
    if (0 == capacity) {
        return NULL;
    } else {
        XCCPageTable_s * newTable = XCCPageTableCreate(capacity);
        if (table) {
            __XCCPageTableCopy(newTable, 0, table, 0, range.location);
            __XCCPageTableCopy(newTable, range.location + range.length, table, range.location, table->count - range.location);
        }

        newTable->capacity = capacity;
        newTable->offset = 0;
        newTable->count = count + range.length;
        return newTable;
    }
}
XCCPageTable_s * _Nullable XCCPageTableCopyByRemove(XCCPageTable_s * _Nullable table, XRange range, XIndex capacity) {
    XIndex count = 0;
    if (table) {
        count = table->count;
    }

    XAssert(range.location < X_BUILD_CollectionCapacityMax, __func__, "");
    XAssert(range.length <= X_BUILD_CollectionCapacityMax, __func__, "");
    XAssert(range.location + range.length <= count, __func__, "");
    XAssert(capacity <= X_BUILD_CollectionCapacityMax, __func__, "");
    XAssert(capacity >= count - range.length, __func__, "");
    
    if (0 == capacity) {
        return NULL;
    } else {
        XCCPageTable_s * newTable = XCCPageTableCreate(capacity);
        if (table) {
            __XCCPageTableCopy(newTable, 0, table, 0, range.location);
            __XCCPageTableCopy(newTable, range.location, table, range.location + range.length, table->count - range.location - range.length);
        }

        newTable->capacity = capacity;
        newTable->offset = 0;
        newTable->count = count + range.length;
        return newTable;
    }
}

void XCCPageTableInsert(XCCPageTable_s * _Nonnull table, XRange range) {
    XAssert(range.location < X_BUILD_CollectionCapacityMax, __func__, "");
    XAssert(range.length <= X_BUILD_CollectionCapacityMax, __func__, "");
    XAssert(table->count + range.length <= X_BUILD_CollectionCapacityMax, __func__, "");
    XAssert(range.location <= table->count, __func__, "");
    __XCCPageTableInsert(table, range);
}
void XCCPageTableRemove(XCCPageTable_s * _Nonnull table, XRange range) {
    XAssert(NULL != table, __func__, "");
    XAssert(range.location < X_BUILD_CollectionCapacityMax, __func__, "");
    XAssert(range.length <= X_BUILD_CollectionCapacityMax, __func__, "");
    XAssert(range.location + range.length <= table->count, __func__, "");
    __XCCPageTableRemove(table, range);
}



#pragma mark - public api

void _XCCPageTableEnumerate(XCCArrayPtr _Nonnull array, XRange range, void * _Nullable context, XCCArrayEnumerateCallBack_f _Nonnull func) {
    XCCPageTable_s * table = array;
    __XCCPageTableEnumerate(table, range, context, func);
}

void _XCCPageTableReverseEnumerate(XCCArrayPtr _Nonnull array, XRange range, void * _Nullable context, XCCArrayEnumerateCallBack_f _Nonnull func) {
    XCCPageTable_s * table = array;
    __XCCPageTableReverseEnumerate(table, range, context, func);
}

XPtr _Nullable XCCPageTableGet(XCCPageTable_s * _Nonnull table, XIndex index) {
    XAssert(NULL != table, __func__, "");
    XAssert(index < table->count, __func__, "");

    return table->pages[index];
}
void XCCPageTableSet(XCCPageTable_s * _Nonnull table, XIndex index, XPtr _Nullable value) {
    XAssert(NULL != table, __func__, "");
    XAssert(index < table->count, __func__, "");
    table->pages[index] = value;
}
