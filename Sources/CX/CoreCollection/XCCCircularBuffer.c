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


static inline XIndex __XCCCircularBufferGoodCapacity(XIndex count) {
    assert(count <= X_BUILD_ArrayCapacityMax);
    XIndex capacity = count;
    XIndex result = 16;
    while (result < capacity) {
        result = result << 1;
    }
    return result;
}

static inline XUInt8 * _Nonnull __XCBufferCreate(XIndex elementSize, XIndex capacity) {
    XUInt8 * newTable = XAlignedAllocate(elementSize * capacity, 64);
    assert(newTable);
    memset(newTable, 0, elementSize * capacity);
    return newTable;
}

static inline XUInt8 * _Nonnull __XCCCircularBufferStorageCreate(XIndex elementSize, XIndex capacity) {
    return __XCBufferCreate(elementSize, capacity);
}
static inline void __XCCCircularBufferStorageDestroy(XUInt8 * _Nonnull page) {
    free(page);
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
static inline void __XCCCircularBufferResizeByInsert(XCCCircularBuffer_s * _Nonnull buffer, XIndex location, XIndex length) {
    XIndex newCount = buffer->_base.count + length;
    XIndex capacity = __XCCCircularBufferGoodCapacity(newCount);
    XIndex elementSize = buffer->_base.elementSize;
    //优先处理 1维 => 1维 1维 => 2维
    if (capacity > buffer->capacity) {
        //拓容
        
        if (0 == buffer->capacity) {
            if (capacity > X_BUILD_DoubleCircularBufferPageCapacity) {
                //0维 => 2维
                XIndex pageCount = (newCount + X_BUILD_DoubleCircularBufferPageCapacity - 1) / X_BUILD_DoubleCircularBufferPageCapacity;
                XIndex pageCapacity = __XCCCircularBufferPageTableGoodCapacity(pageCount);
                XUInt8 * table = __XCCCircularBufferPageTableCreate(pageCapacity);
                buffer->_storage = table;
                buffer->pageCount = pageCount;
                buffer->pageCapacity = pageCapacity;
                buffer->offset.page = 0;
                buffer->offset.item = 0;
                buffer->_base.count = newCount;
                buffer->capacity = capacity;
            } else if (capacity <= X_BUILD_DoubleCircularBufferPageCapacity) {
                //0维 => 1维
                XUInt8 * page = __XCCCircularBufferPageCreate(buffer->_base.elementSize, capacity);
                buffer->_storage = page;
                buffer->pageCount = 0;
                buffer->pageCapacity = 0;
                buffer->offset.page = 0;
                buffer->offset.item = 0;
                buffer->_base.count = newCount;
                buffer->capacity = capacity;
            } else {
                //不可能发生
                abort();
            }
            return;
        } else if (buffer->capacity <= X_BUILD_DoubleCircularBufferPageCapacity) {
            XIndex oldCapacity = buffer->capacity;
            XIndex oldCount = buffer->_base.count;
            XIndex oldOffset = buffer->offset.item;
            XUInt8 * oldPage = buffer->_storage;
            if (capacity > X_BUILD_DoubleCircularBufferPageCapacity) {
                //1维 => 2维
                XIndex pageCount = (newCount + X_BUILD_DoubleCircularBufferPageCapacity - 1) / X_BUILD_DoubleCircularBufferPageCapacity;
                XIndex pageCapacity = __XCCCircularBufferPageTableGoodCapacity(pageCount);
                XUInt8 * table = __XCCCircularBufferPageTableCreate(pageCapacity);
                buffer->_storage = table;
                buffer->pageCount = pageCount;
                buffer->pageCapacity = pageCapacity;
                buffer->offset.page = 0;
                buffer->offset.item = 0;
                buffer->_base.count = newCount;
                buffer->capacity = capacity;
                
                if (location > 0) {
                    XUInt8 * page = XCCCircularBufferPageTableGetPageAtIndex(buffer, 0);//(0 + buffer->offset.item) / X_BUILD_DoubleCircularBufferPageCapacity
                    XCCUtilCircularBufferCopy(page, X_BUILD_DoubleCircularBufferPageCapacity, 0, 0, oldPage, oldCapacity, oldOffset, 0, location, elementSize);
                }
                
                if (location < oldCount) {
                    XIndex remain = oldCount - location;
                    XIndex rbegin = location + length;
                    
                    while (remain > 0) {
                        XIndex pageIndex = rbegin / X_BUILD_DoubleCircularBufferPageCapacity;
                        XIndex end = (pageIndex + 1) * X_BUILD_DoubleCircularBufferPageCapacity;
                        
                        XIndex sliceSize = end - pageIndex;
                        if (sliceSize > remain) {
                            sliceSize = remain;
                        }
                        
                        XUInt8 * page = XCCCircularBufferPageTableGetPageAtIndex(buffer, pageIndex);
                        XCCUtilCircularBufferCopy(page, X_BUILD_DoubleCircularBufferPageCapacity, 0, newCount - remain, oldPage, oldCapacity, oldOffset, oldCount-remain, sliceSize, elementSize);
                        remain -= sliceSize;
                        rbegin += sliceSize;
                    }
                }
                __XCCCircularBufferPageDestroy(oldPage);
                return;
            } else if (capacity <= X_BUILD_DoubleCircularBufferPageCapacity) {
                //1维 => 1维
                XUInt8 * page = __XCCCircularBufferPageCreate(buffer->_base.elementSize, capacity);
                buffer->_storage = page;
                buffer->pageCount = 0;
                buffer->pageCapacity = 0;
                buffer->offset.page = 0;
                buffer->offset.item = 0;
                buffer->_base.count = newCount;
                buffer->capacity = capacity;
                
                if (location > 0) {
                    XCCUtilCircularBufferCopy(page, X_BUILD_DoubleCircularBufferPageCapacity, buffer->offset.item, 0, oldPage, oldCapacity, oldOffset, 0, location, elementSize);
                }
                if (location < oldCount) {
                    XCCUtilCircularBufferCopy(page, X_BUILD_DoubleCircularBufferPageCapacity, buffer->offset.item, location+length, oldPage, oldCapacity, oldOffset, location, oldCount-location, elementSize);
                }
                __XCCCircularBufferPageDestroy(oldPage);
                return;
            } else {
                //不可能发生
                abort();
            }
        } else {
            //不可能发生
            abort();
        }
    } else {
        //移动
        if (capacity > X_BUILD_DoubleCircularBufferPageCapacity) {
            //2维 => 2维
            if (length % X_BUILD_DoubleCircularBufferPageCapacity == 0) {
                //length 是整数页
                XIndex rindex = location + buffer->offset.item;
                XIndex pageIndex = (location + buffer->offset.item) / X_BUILD_DoubleCircularBufferPageCapacity;
                
                //插入页
                __XCCCircularBufferPageTableInsert(buffer, pageIndex, length / X_BUILD_DoubleCircularBufferPageCapacity);

                XIndex remain = rindex % X_BUILD_DoubleCircularBufferPageCapacity;
                if (remain != 0) {
                    //找到location所在的页
                    
                    //页拆分
                    __XCCCircularBufferMoveForward2(buffer, XRangeMake(rindex + length, remain), length);
                }
                return;
            } else {
                //移动
                if (location < buffer->_base.count - location) {
                    //移动前半部分
                    __XCCCircularBufferMoveForward2(buffer, XRangeMake(0, location), length);
                } else {
                    //移动后半部分
                    __XCCCircularBufferMoveBackward2(buffer, XRangeMake(location+length, buffer->_base.count - location), length);
                }
            }
        } else if (capacity <= X_BUILD_DoubleCircularBufferPageCapacity) {
            //1维 => 1维
            
            //移动
            if (location < buffer->_base.count - location) {
                //移动前半部分
                __XCCCircularBufferMoveForward1(buffer, XRangeMake(0, location), length);
            } else {
                //移动后半部分
                __XCCCircularBufferMoveBackward1(buffer, XRangeMake(location+length, buffer->_base.count - location), length);
            }
        } else {
            //不可能发生
            abort();
        }
    }
}


//从location开始删除length个

//0 < length <= table->_base.count
//0 < length + location <= table->_base.count
static inline void __XCCCircularBufferResizeByRemove(XCCCircularBuffer_s * _Nonnull buffer, XIndex location, XIndex length) {
    XIndex capacity = __XCCCircularBufferGoodCapacity(buffer->_base.count - length);
    
    //优先处理 1维 => 1维 2维 => 1维
    if (capacity < buffer->capacity) {
        //可能需要缩容
        
        if (0 == capacity) {
            //缩容
            if (buffer->capacity > X_BUILD_DoubleCircularBufferPageCapacity) {
                //缩容
                //2维 => 0维
                //释放page

                //释放table
            } else if (buffer->capacity <= X_BUILD_DoubleCircularBufferPageCapacity) {
                //1维 => 0维
                //释放page
                
            }  else {
                //不可能发生
                abort();
            }
        } else if (capacity < X_BUILD_DoubleCircularBufferPageCapacity) {
            if (buffer->capacity > X_BUILD_DoubleCircularBufferPageCapacity) {
                //缩容
                //2维 => 1维

            } else {
                //1维 => 1维
                goto doMove;
            }
        } else if (capacity == X_BUILD_DoubleCircularBufferPageCapacity) {
            if (buffer->_base.count - length <= X_BUILD_DoubleCircularBufferPageCapacity * 3 / 4) {
                //缩容
                //2维 => 1维

            } else {
                //暂缓缩容
                //2维 => 2维
                goto doMove;
            }
        } else {
            //不可能发生
            abort();
        }
    }
        
doMove:
    //移动
    if (capacity > X_BUILD_DoubleCircularBufferPageCapacity) {
        //2维 => 2维
        if (length % X_BUILD_DoubleCircularBufferPageCapacity == 0) {
            //length 是整数页
            
            //找到location所在的页
            
            
            //页拆分

            
            //插入页
            
            
            return;
        } else {
            //移动
            
            
            
        }
    } else if (capacity <= X_BUILD_DoubleCircularBufferPageCapacity) {
        //1维 => 1维

        //移动

    } else {
        //不可能发生
        abort();
    };
}

void XCCCircularBufferInsert(XCCDoubleCircularBuffer_s * _Nonnull buffer, XRange range) {
    XAssert(NULL != buffer, __func__, "");
    XAssert(range.location < X_BUILD_ArrayCapacityMax, __func__, "");
    XAssert(range.length <= X_BUILD_ArrayCapacityMax, __func__, "");
    XAssert(buffer->_base.count + range.length <= X_BUILD_ArrayCapacityMax, __func__, "");
    XAssert(range.location <= buffer->_base.count, __func__, "");
    __XCCCircularBufferResizeByInsert(buffer, range.location, range.length);
}
void XCCCircularBufferDelete(XCCDoubleCircularBuffer_s * _Nonnull buffer, XRange range) {
    XAssert(NULL != buffer, __func__, "");
    XAssert(range.location < X_BUILD_ArrayCapacityMax, __func__, "");
    XAssert(range.length <= X_BUILD_ArrayCapacityMax, __func__, "");
    XAssert(range.location + range.length <= buffer->_base.count, __func__, "");
    __XCCCircularBufferResizeByRemove(buffer, range.location, range.length);
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
