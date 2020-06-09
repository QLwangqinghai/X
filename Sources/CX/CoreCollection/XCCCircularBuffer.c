//
//  XCCCircularBuffer.c
//  X
//
//  Created by vector on 2020/6/8.
//  Copyright © 2020 haoqi. All rights reserved.
//

#include "XCCCircularBuffer.h"
#include "XMemory.h"
#include "XCCUtil.h"

static inline XUInt8 * _Nullable __XCCCircularBufferStorageCreate(XIndex elementSize, XIndex capacity) {
    if (0 == capacity) {
        return NULL;
    }
    return XCCAllocate(elementSize, capacity);
}
static inline void __XCCCircularBufferStorageDestroy(XUInt8 * _Nullable page) {
    if (page) {
        free(page);
    }
}


//static inline XIndex __XCCCircularBufferPageTablePrepend(XCCCircularBufferPageTable_s * _Nonnull table, XPtr item) {
//    XAssert(table->count < table->capacity, __func__, "");
//    if (table->count == 0) {
//        table->offset = 0;
//        table->pages[0] = item;
//        table->count += 1;
//    } else {
//        if (table->offset > 0) {
//            table->offset -= 1;
//        } else {
//            table->offset = table->capacity - 1;
//        }
//        table->pages[table->offset] = item;
//        table->count += 1;
//    }
//
//    return 0;
//}
//
//static inline XIndex __XCCCircularBufferPageTableAppend(XCCCircularBufferPageTable_s * _Nonnull table, XPtr item) {
//    XAssert(table->count < table->capacity, __func__, "");
//    if (table->count == 0) {
//        table->offset = 0;
//        table->pages[0] = item;
//        table->count += 1;
//    } else {
//        XIndex idx = table->offset + table->count;
//        if (idx >= table->capacity) {
//            idx -= table->capacity;
//        }
//        table->pages[idx] = item;
//        table->count += 1;
//    }
//    return 0;
//}
//
//static inline XPtr __XCCCircularBufferPageTableRemoveFirst(XCCCircularBufferPageTable_s * _Nonnull table, XIndex capacity) {
//    XAssert(table->count > 0, __func__, "");
//    XPtr result = NULL;
//    XIndex idx = table->offset;
//    result = table->pages[idx];
//    if (table->offset > table->capacity - 1) {
//        abort();
//    } else if (table->offset == table->capacity - 1) {
//        table->offset = 0;
//    } else {
//        table->offset += 1;
//    }
//    table->count -= 1;
//    return result;
//}
//
//static inline XPtr __XCCCircularBufferPageTableRemoveLast(XCCCircularBufferPageTable_s * _Nonnull table, XIndex capacity) {
//    XAssert(table->count > 0, __func__, "");
//    XPtr result = NULL;
//    XIndex idx = table->offset + table->count - 1;
//    if (idx >= table->capacity) {
//        idx -= table->capacity;
//    }
//    result = table->pages[idx];
//    table->count -= 1;
//    return result;
//}

//static inline XCCCircularBufferIndex __XCCCircularBufferLoactionOfIndex(XCCCircularBuffer_s * _Nonnull buffer, XIndex index) {
//    XIndex r = buffer->_capacity - buffer->_offset;
//    XIndex location = 0;
//    if (index >= r) {
//        location = index - r;
//    } else {
//        location = index + buffer->_offset;
//    }
//    return XCCCircularBufferIndexMakeWithIndex(location);
//}
//
//static inline XIndex __XCCCircularBufferIndexOfLoaction(XCCCircularBuffer_s * _Nonnull buffer, XCCCircularBufferIndex location) {
//    XIndex loc = XCCCircularBufferIndexToIndex(location);
//    if (loc >= buffer->_offset) {
//        return loc - buffer->_offset;
//    } else {
//        return buffer->_capacity - (buffer->_offset - loc);
//    }
//}


static inline void __XCCCircularBufferEnumerate(XCCCircularBuffer_s * _Nonnull buffer, XRange range, void * _Nullable context, XCCArrayEnumerateCallBack_f _Nonnull func) {
    
    
}
static inline void __XCCCircularBufferReverseEnumerate(XCCCircularBuffer_s * _Nonnull buffer, XRange range, void * _Nullable context, XCCArrayEnumerateCallBack_f _Nonnull func) {
    
    
}

static inline void __XCCCircularBufferCopy(XCCCircularBuffer_s * _Nonnull to, XIndex toIndex, XCCCircularBuffer_s * _Nonnull from, XIndex fromIndex, XIndex length) {
    if (length > 0) {
        XCCUtilCircularBufferCopy(to->_storage, to->capacity, to->offset, toIndex, from->_storage, from->capacity, from->offset, fromIndex, length, to->_base.elementSize);
    }
}

/**
 * 把range中的元素前移n
 * 前置条件: range.location + range.length <= capacity,   range.location >= n,
 */
void __XCCCircularBufferMoveForward1(XCCCircularBuffer_s * _Nonnull buffer, XRange range, XIndex n) {
    if (range.length <= 0) {
        return;
    }
    
    XCCUtilCircularBufferMoveForward(buffer->_storage, buffer->capacity, buffer->offset, buffer->_base.elementSize, range, n);
}

/**
* 把range中的元素后移n
* 前置条件: range.location + range.length <= capacity,  capacity - (range.location + range.length) >= n
*/
void __XCCCircularBufferMoveBackward1(XCCCircularBuffer_s * _Nonnull buffer, XRange range, XIndex n) {
    if (range.length <= 0) {
        return;
    }
    XCCUtilCircularBufferMoveBackward(buffer->_storage, buffer->capacity, buffer->offset, buffer->_base.elementSize, range, n);
}



//在location位置， 插入length

//0 < length
//0 <= location <= table->_base.count
static inline void __XCCCircularBufferInsert(XCCCircularBuffer_s * _Nonnull buffer, XRange range) {
    XIndex location = range.location;
    XIndex length = range.length;
    XIndex newCount = buffer->_base.count + length;

    //移动
    if (location < buffer->_base.count - location) {
        //移动前半部分
        __XCCCircularBufferMoveForward1(buffer, XRangeMake(0, location), length);
        buffer->_base.count = newCount;
        buffer->offset = (buffer->offset + buffer->capacity - length) % buffer->capacity;
    } else {
        //移动后半部分
        __XCCCircularBufferMoveBackward1(buffer, XRangeMake(location+length, buffer->_base.count - location), length);
        buffer->_base.count = newCount;
    }
    
}


//从location开始删除length个

//0 < length <= table->_base.count
//0 < length + location <= table->_base.count
static inline void __XCCCircularBufferRemove(XCCCircularBuffer_s * _Nonnull buffer, XRange range) {
    XIndex location = range.location;
    XIndex length = range.length;
    XIndex newCount = buffer->_base.count - length;

    //移动
    if (location < buffer->_base.count - location - length) {
        //移动前半部分
        __XCCCircularBufferMoveBackward1(buffer, XRangeMake(0, location), length);
        buffer->_base.count = newCount;
        buffer->offset = (buffer->offset + length) % buffer->capacity;
    } else {
        //移动后半部分
        __XCCCircularBufferMoveForward1(buffer, XRangeMake(location+length, buffer->_base.count - location), length);
        buffer->_base.count = newCount;
    }
}



//在location位置， 插入length

//0 < length
//0 <= location <= table->_base.count
static inline void __XCCCircularBufferInsertResize(XCCCircularBuffer_s * _Nonnull buffer, XRange range, XIndex capacity) {
    if (buffer->capacity == capacity) {
        __XCCCircularBufferInsert(buffer, range);
    } else {
        if (0 == buffer->capacity) {
            XUInt8 * page = __XCCCircularBufferStorageCreate(buffer->_base.elementSize, capacity);
            buffer->_storage = page;
            buffer->capacity = capacity;
            buffer->offset = 0;
            buffer->_base.count = buffer->_base.count + range.length;
        } else {
            XCCCircularBuffer_s old = {};
            memcpy(&old, buffer, sizeof(XCCCircularBuffer_s));
            
            XUInt8 * page = __XCCCircularBufferStorageCreate(buffer->_base.elementSize, capacity);
            buffer->_storage = page;
            buffer->capacity = capacity;
            buffer->offset = 0;
            buffer->_base.count = buffer->_base.count + range.length;
            
            __XCCCircularBufferCopy(buffer, 0, &old, 0, range.location);
            __XCCCircularBufferCopy(buffer, range.location + range.length, &old, range.location, old._base.count - range.location);
            
            __XCCCircularBufferStorageDestroy(old._storage);
        }
    }

}


//从location开始删除length个

//0 < length <= table->_base.count
//0 < length + location <= table->_base.count
static inline void __XCCCircularBufferRemoveResize(XCCCircularBuffer_s * _Nonnull buffer, XRange range, XIndex capacity) {
    
    if (buffer->capacity == capacity) {
        __XCCCircularBufferRemove(buffer, range);
    } else {
        if (0 == buffer->capacity) {
            XUInt8 * page = __XCCCircularBufferStorageCreate(buffer->_base.elementSize, capacity);
            buffer->_storage = page;
            buffer->capacity = capacity;
            buffer->offset = 0;
            buffer->_base.count = buffer->_base.count + range.length;
        } else {
            XCCCircularBuffer_s old = {};
            memcpy(&old, buffer, sizeof(XCCCircularBuffer_s));
            
            XUInt8 * page = __XCCCircularBufferStorageCreate(buffer->_base.elementSize, capacity);
            buffer->_storage = page;
            buffer->capacity = capacity;
            buffer->offset = 0;
            buffer->_base.count = buffer->_base.count - range.length;
            
            __XCCCircularBufferCopy(buffer, 0, &old, 0, range.location);
            __XCCCircularBufferCopy(buffer, range.location, &old, range.location + range.length, old._base.count - range.location - range.length);
            
            __XCCCircularBufferStorageDestroy(old._storage);
        }
    }
}

void XCCCircularBufferInsertResize(XCCCircularBuffer_s * _Nonnull buffer, XRange range, XIndex capacity) {
    XAssert(NULL != buffer, __func__, "");
    XAssert(range.location < X_BUILD_CollectionCapacityMax, __func__, "");
    XAssert(range.length <= X_BUILD_CollectionCapacityMax, __func__, "");
    XAssert(buffer->_base.count + range.length <= X_BUILD_CollectionCapacityMax, __func__, "");
    XAssert(range.location <= buffer->_base.count, __func__, "");
    XAssert(capacity <= X_BUILD_CollectionCapacityMax, __func__, "");
    XAssert(capacity >= buffer->_base.count + range.length, __func__, "");

    __XCCCircularBufferInsertResize(buffer, range, capacity);
}
void XCCCircularBufferRemoveResize(XCCCircularBuffer_s * _Nonnull buffer, XRange range, XIndex capacity) {
    XAssert(NULL != buffer, __func__, "");
    XAssert(range.location < X_BUILD_CollectionCapacityMax, __func__, "");
    XAssert(range.length <= X_BUILD_CollectionCapacityMax, __func__, "");
    XAssert(range.location + range.length <= buffer->_base.count, __func__, "");
    XAssert(capacity <= X_BUILD_CollectionCapacityMax, __func__, "");
    XAssert(capacity >= buffer->_base.count - range.length, __func__, "");
    
    __XCCCircularBufferRemoveResize(buffer, range, capacity);
}


#pragma mark - public api

void _XCCCircularBufferEnumerate(XCCArrayPtr _Nonnull array, XRange range, void * _Nullable context, XCCArrayEnumerateCallBack_f _Nonnull func) {
    XCCCircularBuffer_s * buffer = array;
    __XCCCircularBufferEnumerate(buffer, range, context, func);
}

void _XCCCircularBufferReverseEnumerate(XCCArrayPtr _Nonnull array, XRange range, void * _Nullable context, XCCArrayEnumerateCallBack_f _Nonnull func) {
    XCCCircularBuffer_s * buffer = array;
    __XCCCircularBufferReverseEnumerate(buffer, range, context, func);
}
