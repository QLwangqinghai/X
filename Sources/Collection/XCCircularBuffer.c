//
//  XCCircularBuffer.c
//  X
//
//  Created by vector on 2020/5/31.
//  Copyright © 2020 haoqi. All rights reserved.
//

#include "XCCircularBuffer.h"
#include "XMemory.h"
#include "XCoreCollection.h"


/*
 16-3
 .....
 1024-3

 1024+512*1-3
 1024+512*2-3

 
 
 
 5 13 29 61 125
 
 */

typedef struct __XCCircularBufferPageTable {
    XIndex count;
    XIndex capacity;
    XIndex offset;
    XPtr _Nullable pages[0];
} XCCircularBufferPageTable_s;

static inline void __XCCircularBufferPageTableSet(XCCircularBuffer_s * _Nonnull buffer, XIndex pageIndex, XPtr _Nullable item) {
    XPtr * pages = (XPtr *)(buffer->_storage);
    XIndex loc = (pageIndex + buffer->offset.page) % buffer->pageCapacity;
    pages[loc] = item;
}

static inline XPtr _Nullable __XCCircularBufferPageTableGet(XCCircularBuffer_s * _Nonnull buffer, XIndex pageIndex) {
    XPtr * pages = (XPtr *)(buffer->_storage);
    XIndex loc = (pageIndex + buffer->offset.page) % buffer->pageCapacity;
    return pages[loc];
}

static inline void __XCCircularBufferPageTableMove(XCCircularBuffer_s * _Nonnull buffer, XIndex dst, XIndex src) {
    XPtr * pages = (XPtr *)(buffer->_storage);
    XIndex dstLoc = (dst + buffer->offset.page) % buffer->pageCapacity;
    XIndex srcLoc = (src + buffer->offset.page) % buffer->pageCapacity;
    XAssert(NULL == pages[dstLoc], __func__, "");
    pages[dstLoc] = pages[srcLoc];
    pages[srcLoc] = NULL;
}

XPtr _Nonnull XCCircularBufferPageTableGetPageAtIndex(XCCircularBufferPageTable_s * _Nonnull table);

static inline XIndex __XCCircularBufferPageTableGoodCapacity(XIndex count) {
    assert(count <= X_BUILD_CircularBufferCapacityMax);
    XIndex capacity = count;
    XIndex result = 0;

    if (capacity >= 1024) {
        XIndex m = 1023;
        m = ~m;
        result = (capacity + 1023) & m;
    } else {
        result = 16;
        while (result < capacity) {
            result = result << 1;
        }
    }
    return result;
}

static inline XUInt8 * _Nonnull __XCBufferCreate(XIndex elementSize, XIndex capacity) {
    XUInt8 * newTable = XAlignedAllocate(elementSize * capacity, 64);
    assert(newTable);
    memset(newTable, 0, elementSize * capacity);
    return newTable;
}
static inline XUInt8 * _Nonnull __XCCircularBufferPageCreate(XIndex elementSize, XIndex capacity) {
    return __XCBufferCreate(elementSize, capacity);
}

static inline XUInt8 * _Nonnull __XCCircularBufferPageTableCreate(XIndex capacity) {
    return __XCBufferCreate(sizeof(XPtr), capacity);
}

static inline void __XCCircularBufferPageTableDestroy(XUInt8 * _Nonnull table) {
    free(table);
}

static inline XCCircularBufferPageTable_s * _Nonnull __XCCircularBufferPageTableResize(XCCircularBufferPageTable_s * _Nonnull * _Nonnull tablePtr, XIndex capacity) {
    XIndex goodCapacity = __XCCircularBufferGoodCapacity(capacity);
    
//    if (goodCapacity != table->capacity) {
    
    
//    }
    
    XCCircularBufferPageTable_s * newTable = XAlignedAllocate(sizeof(XCCircularBufferPageTable_s), 64);
    
    return 0;
}

//static inline void __XCCircularBufferPageTableMove(XCCircularBufferPageTable_s * _Nonnull table, XIndex dst, XIndex src) {
//    XIndex count = table->count + length;
//    XIndex capacity = __XCCircularBufferGoodCapacity(count);
//    if (capacity > table->capacity) {
//        //拓容
//        XCCircularBufferPageTable_s * result = __XCCircularBufferPageTableCreate(capacity);
//
//        XIndex count = table->capacity - table->offset;
//        if (count > location) {
//            count = location;
//            for (XIndex idx=0; idx<count; idx++) {
//                result->pages[idx] = table->pages[table->offset + idx];
//
//            }
//
//
//        } else {
//            memcpy(<#void *__dst#>, <#const void *__src#>, <#size_t __n#>)
//
//        }
//        return result;
//    } else {
//
//
//        return NULL;
//    }
//
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
//}


/*
 插入流程 【插入页，移动页，移动页内元素】
 
 移除流程 【移动页内元素，删除页，移动页】

 */


//0 < length
//0 <= location <= table->count
static inline void __XCCircularBufferPageTableInsert(XCCircularBuffer_s * _Nonnull buffer, XIndex location, XIndex length) {
    if (length <= 0) {
        return;
    }
    XIndex newCount = buffer->pageCount + length;
    XIndex capacity = __XCCircularBufferPageTableGoodCapacity(newCount);
    if (capacity > buffer->pageCapacity) {
        //拓容
        XUInt8 * oldPages = buffer->_storage;

        XUInt8 * pages = __XCCircularBufferPageTableCreate(capacity);
        if (location > 0) {
            XCCCircularBufferCopy(pages, capacity, 0, 0, oldPages, buffer->pageCapacity, buffer->offset.page, 0, location, sizeof(void *));
        }
        if (buffer->pageCount > location) {
            XCCCircularBufferCopy(pages, capacity, 0, location + length, oldPages, buffer->pageCapacity, buffer->offset.page, location, buffer->pageCount - location, sizeof(void *));
        }
        buffer->pageCapacity = capacity;
        buffer->pageCount = newCount;
        buffer->offset.page = 0;
        buffer->_storage = pages;
        
        __XCCircularBufferPageTableDestroy(oldPages);
        
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
                XCCCircularBufferMoveForward(buffer->_storage, buffer->pageCapacity, buffer->offset.page, sizeof(XPtr), XRangeMake(length, left), length);
            }
        } else {
            //移动后半部分, 后翻
            buffer->pageCount = newCount;
            if (right > 0) {
                XCCCircularBufferMoveBackward(buffer->_storage, buffer->pageCapacity, buffer->offset.page, sizeof(XPtr), XRangeMake(location, right), length);
            }
        }
    }
}

//0 < length <= table->count
//0 < length + location <= table->count
static inline void __XCCircularBufferPageTableRemove(XCCircularBuffer_s * _Nonnull buffer, XIndex location, XIndex length) {
    XIndex newCount = buffer->pageCount + length;
    XIndex capacity = __XCCircularBufferPageTableGoodCapacity(newCount);
    if (capacity < buffer->pageCapacity) {
        //缩容
        
        XUInt8 * oldPages = buffer->_storage;

        XUInt8 * pages = __XCCircularBufferPageTableCreate(capacity);
        if (location > 0) {
            XCCCircularBufferCopy(pages, capacity, 0, 0, oldPages, buffer->pageCapacity, buffer->offset.page, 0, location, sizeof(XPtr));
        }
        if (buffer->pageCount > location+length) {
            XCCCircularBufferCopy(pages, capacity, 0, location, oldPages, buffer->pageCapacity, buffer->offset.page, location + length, buffer->pageCount - location - length, sizeof(XPtr));
        }
        buffer->pageCapacity = capacity;
        buffer->pageCount = newCount;
        buffer->offset.page = 0;
        buffer->_storage = pages;
        
        __XCCircularBufferPageTableDestroy(oldPages);

    } else {
        XIndex right = buffer->pageCount - location - length;
        XIndex left = location;
        
        if (left < right) {
            //移动前半部分, 后翻 (0, location) => (0+length, )
            if (left > 0) {
                XCCCircularBufferMoveBackward(buffer->_storage, buffer->pageCapacity, buffer->offset.page, sizeof(XPtr), XRangeMake(0, left), length);
            }
            buffer->offset.page = (buffer->offset.page + length) % buffer->capacity;
            buffer->pageCount = newCount;
        } else {
            //移动后半部分, 前翻 (location+length, oldCount - right) => (location, )
            
            XCCCircularBufferMoveForward(buffer->_storage, buffer->pageCapacity, buffer->offset.page, sizeof(XPtr), XRangeMake(location+length, right), buffer->pageCount - right);
            buffer->pageCount = newCount;
        }
    }

}
static inline XIndex __XCCircularBufferPageTablePrepend(XCCircularBufferPageTable_s * _Nonnull table, XPtr item) {
    XAssert(table->count < table->capacity, __func__, "");
    if (table->count == 0) {
        table->offset = 0;
        table->pages[0] = item;
        table->count += 1;
    } else {
        if (table->offset > 0) {
            table->offset -= 1;
        } else {
            table->offset = table->capacity - 1;
        }
        table->pages[table->offset] = item;
        table->count += 1;
    }

    return 0;
}

static inline XIndex __XCCircularBufferPageTableAppend(XCCircularBufferPageTable_s * _Nonnull table, XPtr item) {
    XAssert(table->count < table->capacity, __func__, "");
    if (table->count == 0) {
        table->offset = 0;
        table->pages[0] = item;
        table->count += 1;
    } else {
        XIndex idx = table->offset + table->count;
        if (idx >= table->capacity) {
            idx -= table->capacity;
        }
        table->pages[idx] = item;
        table->count += 1;
    }
    return 0;
}

static inline XPtr __XCCircularBufferPageTableRemoveFirst(XCCircularBufferPageTable_s * _Nonnull table, XIndex capacity) {
    XAssert(table->count > 0, __func__, "");
    XPtr result = NULL;
    XIndex idx = table->offset;
    result = table->pages[idx];
    if (table->offset > table->capacity - 1) {
        abort();
    } else if (table->offset == table->capacity - 1) {
        table->offset = 0;
    } else {
        table->offset += 1;
    }
    table->count -= 1;
    return result;
}

static inline XPtr __XCCircularBufferPageTableRemoveLast(XCCircularBufferPageTable_s * _Nonnull table, XIndex capacity) {
    XAssert(table->count > 0, __func__, "");
    XPtr result = NULL;
    XIndex idx = table->offset + table->count - 1;
    if (idx >= table->capacity) {
        idx -= table->capacity;
    }
    result = table->pages[idx];
    table->count -= 1;
    return result;
}

//static inline XCCircularBufferIndex __XCCircularBufferLoactionOfIndex(XCCircularBuffer_s * _Nonnull buffer, XIndex index) {
//    XIndex r = buffer->_capacity - buffer->_offset;
//    XIndex location = 0;
//    if (index >= r) {
//        location = index - r;
//    } else {
//        location = index + buffer->_offset;
//    }
//    return XCCircularBufferIndexMakeWithIndex(location);
//}
//
//static inline XIndex __XCCircularBufferIndexOfLoaction(XCCircularBuffer_s * _Nonnull buffer, XCCircularBufferIndex location) {
//    XIndex loc = XCCircularBufferIndexToIndex(location);
//    if (loc >= buffer->_offset) {
//        return loc - buffer->_offset;
//    } else {
//        return buffer->_capacity - (buffer->_offset - loc);
//    }
//}


static inline void __XCCircularBufferEnumerate(XCCircularBuffer_s * _Nonnull buffer, XRange range, void * _Nullable context, XCArrayEnumerateCallBack_f _Nonnull func) {
    
    
}
static inline void __XCCircularBufferReverseEnumerate(XCCircularBuffer_s * _Nonnull buffer, XRange range, void * _Nullable context, XCArrayEnumerateCallBack_f _Nonnull func) {
    
    
}
void _XCCircularBufferEnumerate(XCArrayPtr * _Nonnull array, XRange range, void * _Nullable context, XCArrayEnumerateCallBack_f _Nonnull func) {
    
    
}

void _XCCircularBufferReverseEnumerate(XCArrayPtr _Nonnull array, XRange range, void * _Nullable context, XCArrayEnumerateCallBack_f _Nonnull func) {
    
    
}


/**
 * 把range中的元素前移n
 * 前置条件: range.location + range.length <= capacity,   range.location >= n,
 */
void __XCCircularBufferMoveForward1(XCCircularBuffer_s * _Nonnull buffer, XRange range, XIndex n) {
    if (range.length <= 0) {
        return;
    }
    
    
}

/**
* 把range中的元素后移n
* 前置条件: range.location + range.length <= capacity,  capacity - (range.location + range.length) >= n
*/
void __XCCircularBufferMoveBackward1(XCCircularBuffer_s * _Nonnull buffer, XRange range, XIndex n) {
    if (range.length <= 0) {
        return;
    }
    
}

/**
 * 把range中的元素前移n
 * 前置条件: range.location + range.length <= capacity,   range.location >= n,
 */
void __XCCircularBufferMoveForward2(XCCircularBuffer_s * _Nonnull buffer, XRange range, XIndex n) {
    if (range.length <= 0) {
        return;
    }
    
}

/**
* 把range中的元素后移n
* 前置条件: range.location + range.length <= capacity,  capacity - (range.location + range.length) >= n
*/
void __XCCircularBufferMoveBackward2(XCCircularBuffer_s * _Nonnull buffer, XRange range, XIndex n) {
    if (range.length <= 0) {
        return;
    }
    
}



//在location位置， 插入length

//0 < length
//0 <= location <= table->_base.count
static inline void __XCCircularBufferResizeByInsert(XCCircularBuffer_s * _Nonnull buffer, XIndex location, XIndex length) {
    XIndex newCount = buffer->_base.count + length;
    XIndex capacity = __XCCircularBufferGoodCapacity(newCount);
    //优先处理 1维 => 1维 1维 => 2维
    if (capacity > buffer->capacity) {
        //拓容
        
        if (0 == buffer->capacity) {
            if (capacity > X_BUILD_CircularBufferPageCapacity) {
                //0维 => 2维
                XIndex pageCount = (newCount + X_BUILD_CircularBufferPageCapacity - 1) / X_BUILD_CircularBufferPageCapacity;
                XIndex pageCapacity = __XCCircularBufferPageTableGoodCapacity(pageCount);
                XUInt8 * table = __XCCircularBufferPageTableCreate(pageCapacity);
                buffer->_storage = table;
                buffer->pageCount = pageCount;
                buffer->pageCapacity = pageCapacity;
                buffer->offset.page = 0;
                buffer->offset.item = 0;
                buffer->_base.count = newCount;
                buffer->capacity = capacity;
            } else if (capacity <= X_BUILD_CircularBufferPageCapacity) {
                //0维 => 1维
                XUInt8 * page = __XCCircularBufferPageCreate(buffer->_base.elementSize, capacity);
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
        } else if (buffer->capacity <= X_BUILD_CircularBufferPageCapacity) {
            XIndex oldCapacity = buffer->capacity;
            XIndex oldCount = buffer->_base.count;
            XIndex oldOffset = buffer->offset.item;
            XUInt8 * oldPage = buffer->_storage;
            if (capacity > X_BUILD_CircularBufferPageCapacity) {
                //1维 => 2维
                XIndex pageCount = (newCount + X_BUILD_CircularBufferPageCapacity - 1) / X_BUILD_CircularBufferPageCapacity;
                XIndex pageCapacity = __XCCircularBufferPageTableGoodCapacity(pageCount);
                XUInt8 * table = __XCCircularBufferPageTableCreate(pageCapacity);
                buffer->_storage = table;
                buffer->pageCount = pageCount;
                buffer->pageCapacity = pageCapacity;
                buffer->offset.page = 0;
                buffer->offset.item = 0;
                buffer->_base.count = newCount;
                buffer->capacity = capacity;
                
                
                
                
                return;
            } else if (capacity <= X_BUILD_CircularBufferPageCapacity) {
                //1维 => 1维
                XUInt8 * page = __XCCircularBufferPageCreate(buffer->_base.elementSize, capacity);
                buffer->_storage = page;
                buffer->pageCount = 0;
                buffer->pageCapacity = 0;
                buffer->offset.page = 0;
                buffer->offset.item = 0;
                buffer->_base.count = newCount;
                buffer->capacity = capacity;
                
                
                
                
                
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
        if (capacity > X_BUILD_CircularBufferPageCapacity) {
            //2维 => 2维
            if (length % X_BUILD_CircularBufferPageCapacity == 0) {
                //length 是整数页
                
                //找到location所在的页
                
                
                //页拆分

                
                //插入页
                
                
                return;
            } else {
                //移动
                if (location < buffer->_base.count - location) {
                    //移动前半部分
                    __XCCircularBufferMoveForward2(buffer, XRangeMake(0, location), length);
                } else {
                    //移动后半部分
                    __XCCircularBufferMoveBackward2(buffer, XRangeMake(location+length, buffer->_base.count - location), length);
                }
            }
        } else if (capacity <= X_BUILD_CircularBufferPageCapacity) {
            //1维 => 1维
            XIndex oldCapacity = buffer->capacity;
            XIndex oldCount = buffer->_base.count;
            XIndex oldOffset = buffer->offset.item;
            XUInt8 * oldPage = buffer->_storage;
            
            //移动
            if (location < buffer->_base.count - location) {
                //移动前半部分
                __XCCircularBufferMoveForward1(buffer, XRangeMake(0, location), length);
            } else {
                //移动后半部分
                __XCCircularBufferMoveBackward1(buffer, XRangeMake(location+length, buffer->_base.count - location), length);
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
static inline void __XCCircularBufferResizeByRemove(XCCircularBuffer_s * _Nonnull buffer, XIndex location, XIndex length) {
    XIndex capacity = __XCCircularBufferGoodCapacity(buffer->_base.count - length);
    

    //优先处理 1维 => 1维 2维 => 1维
    if (capacity < buffer->capacity) {
        //可能需要缩容
        
        if (0 == capacity) {
            //缩容
            if (buffer->capacity > X_BUILD_CircularBufferPageCapacity) {
                //缩容
                //2维 => 0维
                //释放page

                //释放table
            } else if (buffer->capacity <= X_BUILD_CircularBufferPageCapacity) {
                //1维 => 0维
                //释放page
                
            }  else {
                //不可能发生
                abort();
            }
        } else if (capacity < X_BUILD_CircularBufferPageCapacity) {
            if (buffer->capacity > X_BUILD_CircularBufferPageCapacity) {
                //缩容
                //2维 => 1维

            } else {
                //1维 => 1维
                goto doMove;
            }
        } else if (capacity == X_BUILD_CircularBufferPageCapacity) {
            if (buffer->_base.count - length <= X_BUILD_CircularBufferPageCapacity * 3 / 4) {
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
    if (capacity > X_BUILD_CircularBufferPageCapacity) {
        //2维 => 2维
        if (length % X_BUILD_CircularBufferPageCapacity == 0) {
            //length 是整数页
            
            //找到location所在的页
            
            
            //页拆分

            
            //插入页
            
            
            return;
        } else {
            //移动
            
            
            
        }
    } else if (capacity <= X_BUILD_CircularBufferPageCapacity) {
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
void XCCircularBufferReplace(XCCircularBuffer_s * _Nonnull buffer, XCArrayPtr _Nullable source, XRange range) {
    XAssert(NULL != buffer, __func__, "");
    XAssert(range.location < X_BUILD_CircularBufferCapacityMax, __func__, "");
    XAssert(range.length <= X_BUILD_CircularBufferCapacityMax, __func__, "");
    XAssert(range.location + range.length <= buffer->_base.count, __func__, "");

    XCCircularBuffer_s * result = NULL;
    size_t elementSize = buffer->_base.elementSize;

    //插入或者删除
    XIndex sourceCount = 0;
    if (source) {
        sourceCount = XCArrayGetCount(source);
    }
    if (sourceCount > range.length) {
        //insert
        __XCCircularBufferResizeByInsert(buffer, range.location + range.length, sourceCount - range.length);
    } else if (sourceCount < range.length) {
        //remove
        __XCCircularBufferResizeByRemove(buffer, range.location + sourceCount, range.length - sourceCount);
    }
    
    
    //move
    
    
    
    //insert
    
}







