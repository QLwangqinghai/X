//
//  XCCDoubleCircularBuffer.c
//  X
//
//  Created by vector on 2020/5/31.
//  Copyright © 2020 haoqi. All rights reserved.
//

#include "XCCDoubleCircularBuffer.h"
#include "XMemory.h"
#include "XCCBase.h"
#include "XCCUtil.h"





static inline XIndex __XCCDoubleCircularBufferPageTableGoodCapacity(XIndex count) {
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

static inline XUInt8 * _Nonnull __XCCDoubleCircularBufferPageCreate(XIndex elementSize, XIndex capacity) {
    return __XCBufferCreate(elementSize, capacity);
}

static inline XUInt8 * _Nonnull __XCCDoubleCircularBufferPageTableCreate(XIndex capacity) {
    return __XCBufferCreate(sizeof(XPtr), capacity);
}

static inline void __XCCDoubleCircularBufferPageTableDestroy(XUInt8 * _Nonnull table, XIndex capacity, XIndex offset, XIndex count) {
    XPtr * pages = (XPtr *)table;

    free(table);
}

static inline void __XCCDoubleCircularBufferPageDestroy(XUInt8 * _Nonnull page) {
    free(page);
}

static inline XUInt8 * _Nonnull XCCDoubleCircularBufferPageTableGetPageAtIndex(XCCDoubleCircularBuffer_s * _Nonnull buffer, XIndex index) {
    XAssert(index < buffer->pageCount, __func__, "");
    XPtr * pages = (XPtr *)(buffer->_storage);
    XIndex loc = (index + buffer->offset.page) % buffer->pageCapacity;
    XUInt8 * page = pages[loc];
    if (NULL == page) {
        page = __XCCDoubleCircularBufferPageCreate(buffer->_base.elementSize, X_BUILD_DoubleCircularBufferPageCapacity);
        pages[loc] = page;
    }
    return page;
}

/*
 插入流程 【插入页，移动页，移动页内元素】
 
 移除流程 【移动页内元素，删除页，移动页】

 */


//0 < length
//0 <= location <= table->count
static inline void __XCCDoubleCircularBufferPageTableInsert(XCCDoubleCircularBuffer_s * _Nonnull buffer, XIndex location, XIndex length) {
    if (length <= 0) {
        return;
    }
    XIndex newCount = buffer->pageCount + length;
    XIndex capacity = __XCCDoubleCircularBufferPageTableGoodCapacity(newCount);
    XIndex oldCapacity = buffer->pageCapacity;
    XIndex oldCount = buffer->pageCount;
    XIndex oldOffset = buffer->offset.page;

    if (capacity > oldCapacity) {
        //拓容
        XUInt8 * oldPages = buffer->_storage;

        XUInt8 * pages = __XCCDoubleCircularBufferPageTableCreate(capacity);
        if (location > 0) {
            XCCUtilCircularBufferCopy(pages, capacity, 0, 0, oldPages, buffer->pageCapacity, buffer->offset.page, 0, location, sizeof(void *));
        }
        if (buffer->pageCount > location) {
            XCCUtilCircularBufferCopy(pages, capacity, 0, location + length, oldPages, buffer->pageCapacity, buffer->offset.page, location, buffer->pageCount - location, sizeof(void *));
        }
        buffer->pageCapacity = capacity;
        buffer->pageCount = newCount;
        buffer->offset.page = 0;
        buffer->_storage = pages;
        
        __XCCDoubleCircularBufferPageTableDestroy(oldPages, oldCapacity, oldOffset, oldCount);
    } else {
        XIndex right = buffer->pageCount - location;
        XIndex left = location;
        if (left < right) {
            //移动前半部分, 前翻
            if (buffer->offset.page >= length) {
                buffer->offset.page -= length;
            } else {
                buffer->offset.page = buffer->offset.page + buffer->pageCapacity - length;
            }
            buffer->pageCount = newCount;
            if (left > 0) {
                XCCUtilCircularBufferMoveForward(buffer->_storage, buffer->pageCapacity, buffer->offset.page, sizeof(XPtr), XRangeMake(length, left), length);
            }
        } else {
            //移动后半部分, 后翻
            buffer->pageCount = newCount;
            if (right > 0) {
                XCCUtilCircularBufferMoveBackward(buffer->_storage, buffer->pageCapacity, buffer->offset.page, sizeof(XPtr), XRangeMake(location, right), length);
            }
        }
    }
}

//0 < length <= table->count
//0 < length + location <= table->count
static inline void __XCCDoubleCircularBufferPageTableRemove(XCCDoubleCircularBuffer_s * _Nonnull buffer, XIndex location, XIndex length) {
    XIndex oldCount = buffer->pageCount;
    XIndex newCount = oldCount + length;

    XIndex capacity = __XCCDoubleCircularBufferPageTableGoodCapacity(newCount);
    XIndex oldCapacity = buffer->pageCapacity;
    XIndex oldOffset = buffer->offset.page;
    if (capacity < oldCapacity) {
        //缩容
        
        XUInt8 * oldPages = buffer->_storage;

        XUInt8 * pages = __XCCDoubleCircularBufferPageTableCreate(capacity);
        if (location > 0) {
            XCCUtilCircularBufferCopy(pages, capacity, 0, 0, oldPages, buffer->pageCapacity, buffer->offset.page, 0, location, sizeof(XPtr));
        }
        if (buffer->pageCount > location+length) {
            XCCUtilCircularBufferCopy(pages, capacity, 0, location, oldPages, buffer->pageCapacity, buffer->offset.page, location + length, buffer->pageCount - location - length, sizeof(XPtr));
        }
        buffer->pageCapacity = capacity;
        buffer->pageCount = newCount;
        buffer->offset.page = 0;
        buffer->_storage = pages;
        
        __XCCDoubleCircularBufferPageTableDestroy(oldPages, oldCapacity, oldOffset, oldCount);

    } else {
        XIndex right = buffer->pageCount - location - length;
        XIndex left = location;
        
        if (left < right) {
            //移动前半部分, 后翻 (0, location) => (0+length, )
            if (left > 0) {
                XCCUtilCircularBufferMoveBackward(buffer->_storage, buffer->pageCapacity, buffer->offset.page, sizeof(XPtr), XRangeMake(0, left), length);
            }
            buffer->offset.page = (buffer->offset.page + length) % buffer->capacity;
            buffer->pageCount = newCount;
        } else {
            //移动后半部分, 前翻 (location+length, oldCount - right) => (location, )
            
            XCCUtilCircularBufferMoveForward(buffer->_storage, buffer->pageCapacity, buffer->offset.page, sizeof(XPtr), XRangeMake(location+length, right), buffer->pageCount - right);
            buffer->pageCount = newCount;
        }
    }

}
//static inline XIndex __XCCDoubleCircularBufferPageTablePrepend(XCCDoubleCircularBufferPageTable_s * _Nonnull table, XPtr item) {
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
//static inline XIndex __XCCDoubleCircularBufferPageTableAppend(XCCDoubleCircularBufferPageTable_s * _Nonnull table, XPtr item) {
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
//static inline XPtr __XCCDoubleCircularBufferPageTableRemoveFirst(XCCDoubleCircularBufferPageTable_s * _Nonnull table, XIndex capacity) {
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
//static inline XPtr __XCCDoubleCircularBufferPageTableRemoveLast(XCCDoubleCircularBufferPageTable_s * _Nonnull table, XIndex capacity) {
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

//static inline XCCDoubleCircularBufferIndex __XCCDoubleCircularBufferLoactionOfIndex(XCCDoubleCircularBuffer_s * _Nonnull buffer, XIndex index) {
//    XIndex r = buffer->_capacity - buffer->_offset;
//    XIndex location = 0;
//    if (index >= r) {
//        location = index - r;
//    } else {
//        location = index + buffer->_offset;
//    }
//    return XCCDoubleCircularBufferIndexMakeWithIndex(location);
//}
//
//static inline XIndex __XCCDoubleCircularBufferIndexOfLoaction(XCCDoubleCircularBuffer_s * _Nonnull buffer, XCCDoubleCircularBufferIndex location) {
//    XIndex loc = XCCDoubleCircularBufferIndexToIndex(location);
//    if (loc >= buffer->_offset) {
//        return loc - buffer->_offset;
//    } else {
//        return buffer->_capacity - (buffer->_offset - loc);
//    }
//}


static inline void __XCCDoubleCircularBufferEnumerate(XCCDoubleCircularBuffer_s * _Nonnull buffer, XRange range, void * _Nullable context, XCCArrayEnumerateCallBack_f _Nonnull func) {
    
    
}
static inline void __XCCDoubleCircularBufferReverseEnumerate(XCCDoubleCircularBuffer_s * _Nonnull buffer, XRange range, void * _Nullable context, XCCArrayEnumerateCallBack_f _Nonnull func) {
    
    
}



/**
 * 把range中的元素前移n
 * 前置条件: range.location + range.length <= capacity,   range.location >= n,
 */
void __XCCDoubleCircularBufferMoveForward1(XCCDoubleCircularBuffer_s * _Nonnull buffer, XRange range, XIndex n) {
    if (range.length <= 0) {
        return;
    }
    
    XCCUtilCircularBufferMoveForward(buffer->_storage, buffer->capacity, buffer->offset.item, buffer->_base.elementSize, range, n);
}

/**
* 把range中的元素后移n
* 前置条件: range.location + range.length <= capacity,  capacity - (range.location + range.length) >= n
*/
void __XCCDoubleCircularBufferMoveBackward1(XCCDoubleCircularBuffer_s * _Nonnull buffer, XRange range, XIndex n) {
    if (range.length <= 0) {
        return;
    }
    XCCUtilCircularBufferMoveBackward(buffer->_storage, buffer->capacity, buffer->offset.item, buffer->_base.elementSize, range, n);
}

/**
 * 把range中的元素前移n
 * 前置条件: range.location + range.length <= capacity,   range.location >= n,
 */
void __XCCDoubleCircularBufferMoveForward2(XCCDoubleCircularBuffer_s * _Nonnull buffer, XRange range, XIndex n) {
    if (range.length <= 0) {
        return;
    }
    
}

/**
* 把range中的元素后移n
* 前置条件: range.location + range.length <= capacity,  capacity - (range.location + range.length) >= n
*/
void __XCCDoubleCircularBufferMoveBackward2(XCCDoubleCircularBuffer_s * _Nonnull buffer, XRange range, XIndex n) {
    if (range.length <= 0) {
        return;
    }
    
}



//在location位置， 插入length

//0 < length
//0 <= location <= table->_base.count
static inline void __XCCDoubleCircularBufferResizeByInsert(XCCDoubleCircularBuffer_s * _Nonnull buffer, XIndex location, XIndex length) {
    XIndex newCount = buffer->_base.count + length;
    XIndex capacity = __XCCDoubleCircularBufferGoodCapacity(newCount);
    XIndex elementSize = buffer->_base.elementSize;
    //优先处理 1维 => 1维 1维 => 2维
    if (capacity > buffer->capacity) {
        //拓容
        
        if (0 == buffer->capacity) {
            if (capacity > X_BUILD_DoubleCircularBufferPageCapacity) {
                //0维 => 2维
                XIndex pageCount = (newCount + X_BUILD_DoubleCircularBufferPageCapacity - 1) / X_BUILD_DoubleCircularBufferPageCapacity;
                XIndex pageCapacity = __XCCDoubleCircularBufferPageTableGoodCapacity(pageCount);
                XUInt8 * table = __XCCDoubleCircularBufferPageTableCreate(pageCapacity);
                buffer->_storage = table;
                buffer->pageCount = pageCount;
                buffer->pageCapacity = pageCapacity;
                buffer->offset.page = 0;
                buffer->offset.item = 0;
                buffer->_base.count = newCount;
                buffer->capacity = capacity;
            } else if (capacity <= X_BUILD_DoubleCircularBufferPageCapacity) {
                //0维 => 1维
                XUInt8 * page = __XCCDoubleCircularBufferPageCreate(buffer->_base.elementSize, capacity);
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
                XIndex pageCapacity = __XCCDoubleCircularBufferPageTableGoodCapacity(pageCount);
                XUInt8 * table = __XCCDoubleCircularBufferPageTableCreate(pageCapacity);
                buffer->_storage = table;
                buffer->pageCount = pageCount;
                buffer->pageCapacity = pageCapacity;
                buffer->offset.page = 0;
                buffer->offset.item = 0;
                buffer->_base.count = newCount;
                buffer->capacity = capacity;
                
                if (location > 0) {
                    XUInt8 * page = XCCDoubleCircularBufferPageTableGetPageAtIndex(buffer, 0);//(0 + buffer->offset.item) / X_BUILD_DoubleCircularBufferPageCapacity
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
                        
                        XUInt8 * page = XCCDoubleCircularBufferPageTableGetPageAtIndex(buffer, pageIndex);
                        XCCUtilCircularBufferCopy(page, X_BUILD_DoubleCircularBufferPageCapacity, 0, newCount - remain, oldPage, oldCapacity, oldOffset, oldCount-remain, sliceSize, elementSize);
                        remain -= sliceSize;
                        rbegin += sliceSize;
                    }
                }
                __XCCDoubleCircularBufferPageDestroy(oldPage);
                return;
            } else if (capacity <= X_BUILD_DoubleCircularBufferPageCapacity) {
                //1维 => 1维
                XUInt8 * page = __XCCDoubleCircularBufferPageCreate(buffer->_base.elementSize, capacity);
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
                __XCCDoubleCircularBufferPageDestroy(oldPage);
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
                __XCCDoubleCircularBufferPageTableInsert(buffer, pageIndex, length / X_BUILD_DoubleCircularBufferPageCapacity);

                XIndex remain = rindex % X_BUILD_DoubleCircularBufferPageCapacity;
                if (remain != 0) {
                    //找到location所在的页
                    
                    //页拆分
                    __XCCDoubleCircularBufferMoveForward2(buffer, XRangeMake(rindex + length, remain), length);
                }
                return;
            } else {
                //移动
                if (location < buffer->_base.count - location) {
                    //移动前半部分
                    __XCCDoubleCircularBufferMoveForward2(buffer, XRangeMake(0, location), length);
                } else {
                    //移动后半部分
                    __XCCDoubleCircularBufferMoveBackward2(buffer, XRangeMake(location+length, buffer->_base.count - location), length);
                }
            }
        } else if (capacity <= X_BUILD_DoubleCircularBufferPageCapacity) {
            //1维 => 1维
            
            //移动
            if (location < buffer->_base.count - location) {
                //移动前半部分
                __XCCDoubleCircularBufferMoveForward1(buffer, XRangeMake(0, location), length);
                
                buffer->_base.count = newCount;
                buffer->offset.item = (buffer->offset.item + X_BUILD_DoubleCircularBufferPageCapacity - length) % X_BUILD_DoubleCircularBufferPageCapacity;
            } else {
                //移动后半部分
                __XCCDoubleCircularBufferMoveBackward1(buffer, XRangeMake(location+length, buffer->_base.count - location), length);
                buffer->_base.count = newCount;
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
static inline void __XCCDoubleCircularBufferResizeByRemove(XCCDoubleCircularBuffer_s * _Nonnull buffer, XIndex location, XIndex length) {
    XIndex capacity = __XCCDoubleCircularBufferGoodCapacity(buffer->_base.count - length);
    
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





//resize == 0 时， 不进行resize
// This function does no ObjC dispatch or argument checking;
// It should only be called from places where that dispatch and check has already been done, or NSCCArray
void XCCDoubleCircularBufferReplace(XCCDoubleCircularBuffer_s * _Nonnull buffer, XCCArrayPtr _Nullable source, XRange range) {
    XAssert(NULL != buffer, __func__, "");
    XAssert(range.location < X_BUILD_ArrayCapacityMax, __func__, "");
    XAssert(range.length <= X_BUILD_ArrayCapacityMax, __func__, "");
    XAssert(range.location + range.length <= buffer->_base.count, __func__, "");

    XCCDoubleCircularBuffer_s * result = NULL;
    size_t elementSize = buffer->_base.elementSize;

    

    
    //插入或者删除
    XIndex sourceCount = 0;
    if (source) {
        sourceCount = XCArrayGetCount(source);
    }
    if (sourceCount > range.length) {
        //insert
        __XCCDoubleCircularBufferResizeByInsert(buffer, range.location + range.length, sourceCount - range.length);
    } else if (sourceCount < range.length) {
        //remove
        __XCCDoubleCircularBufferResizeByRemove(buffer, range.location + sourceCount, range.length - sourceCount);
    }
    
    
    //move
    
    
    
    //insert
    
}

void XCCDoubleCircularBufferInsert(XCCDoubleCircularBuffer_s * _Nonnull buffer, XRange range) {
    XAssert(NULL != buffer, __func__, "");
    XAssert(range.location < X_BUILD_ArrayCapacityMax, __func__, "");
    XAssert(range.length <= X_BUILD_ArrayCapacityMax, __func__, "");
    XAssert(buffer->_base.count + range.length <= X_BUILD_ArrayCapacityMax, __func__, "");
    XAssert(range.location <= buffer->_base.count, __func__, "");
    __XCCDoubleCircularBufferResizeByInsert(buffer, range.location, range.length);
}
void XCCDoubleCircularBufferDelete(XCCDoubleCircularBuffer_s * _Nonnull buffer, XRange range) {
    XAssert(NULL != buffer, __func__, "");
    XAssert(range.location < X_BUILD_ArrayCapacityMax, __func__, "");
    XAssert(range.length <= X_BUILD_ArrayCapacityMax, __func__, "");
    XAssert(range.location + range.length <= buffer->_base.count, __func__, "");
    __XCCDoubleCircularBufferResizeByRemove(buffer, range.location, range.length);
}




#pragma mark - public api

void _XCCDoubleCircularBufferEnumerate(XCCArrayPtr _Nonnull array, XRange range, void * _Nullable context, XCCArrayEnumerateCallBack_f _Nonnull func) {
    XCCDoubleCircularBuffer_s * buffer = array;
    __XCCDoubleCircularBufferEnumerate(buffer, range, context, func);

    
}

void _XCCDoubleCircularBufferReverseEnumerate(XCCArrayPtr _Nonnull array, XRange range, void * _Nullable context, XCCArrayEnumerateCallBack_f _Nonnull func) {
    XCCDoubleCircularBuffer_s * buffer = array;
    __XCCDoubleCircularBufferReverseEnumerate(buffer, range, context, func);
}
