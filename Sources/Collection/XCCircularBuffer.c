//
//  XCCircularBuffer.c
//  X
//
//  Created by vector on 2020/5/31.
//  Copyright © 2020 haoqi. All rights reserved.
//

#include "XCCircularBuffer.h"
#include "XMemory.h"

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

XPtr _Nonnull XCCircularBufferPageTableGetPageAtIndex(XCCircularBufferPageTable_s * _Nonnull table);

static inline XIndex __XCCircularBufferPageTableGoodCapacity(XIndex capacity) {
    assert(capacity <= X_BUILD_CircularBufferCapacityMax);
    capacity += 3;
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

    result -= 3;
    return result;
}

static inline XPtr _Nonnull __XCCircularBufferPageCreate(XIndex elementSize, XIndex capacity) {
    XCCircularBufferPageTable_s * newTable = XAlignedAllocate(elementSize * capacity, 64);
    assert(newTable);
    newTable->capacity = capacity;
    newTable->count = 0;
    newTable->offset = 0;
    memset(&(newTable->pages[0]), 0, sizeof(XPtr) * capacity);
    return newTable;
}

static inline XCCircularBufferPageTable_s * _Nonnull __XCCircularBufferPageTableCreate(XIndex capacity) {
    XCCircularBufferPageTable_s * newTable = XAlignedAllocate(sizeof(XCCircularBufferPageTable_s) + sizeof(XPtr) * capacity, 64);
    assert(newTable);
    newTable->capacity = capacity;
    newTable->count = 0;
    newTable->offset = 0;
    memset(&(newTable->pages[0]), 0, sizeof(XPtr) * capacity);
    return newTable;
}

static inline void __XCCircularBufferPageTableDestroy(XCCircularBufferPageTable_s * _Nonnull table) {
    assert(table->count == 0);
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

static inline void __XCCircularBufferPageTableSet(XCCircularBufferPageTable_s * _Nonnull table, XIndex index, XPtr _Nullable item) {
    XIndex loc = (index + table->offset) % table->capacity;
    table->pages[loc] = item;
}

static inline XPtr _Nullable __XCCircularBufferPageTableGet(XCCircularBufferPageTable_s * _Nonnull table, XIndex index) {
    XIndex loc = (index + table->offset) % table->capacity;
    return table->pages[loc];
}

static inline void __XCCircularBufferPageTableAdd(XCCircularBufferPageTable_s * _Nonnull table, XPtr _Nullable item) {
    XIndex index = table->count;
    XIndex loc = (index + table->offset) % table->capacity;
    table->pages[loc] = item;
    table->count += 1;
}

//0 < length
//0 <= location <= table->count
static inline XCCircularBufferPageTable_s * _Nullable __XCCircularBufferPageTableInsert(XCCircularBufferPageTable_s * _Nonnull table, XIndex location, XIndex length) {
    if (length <= 0) {
        return NULL;
    }
    XIndex count = table->count + length;
    XIndex capacity = __XCCircularBufferGoodCapacity(count);
    if (capacity > table->capacity) {
        //拓容
        XCCircularBufferPageTable_s * result = __XCCircularBufferPageTableCreate(capacity);
        for (XIndex idx=0; idx<location; idx++) {
            __XCCircularBufferPageTableAdd(result, __XCCircularBufferPageTableGet(table, idx));
        }
        for (XIndex idx=0; idx<length; idx++) {
            __XCCircularBufferPageTableAdd(result, NULL);
        }
        for (XIndex idx=location; idx<table->count; idx++) {
            __XCCircularBufferPageTableAdd(result, __XCCircularBufferPageTableGet(table, idx));
        }
        return result;
    } else {
        XIndex right = table->count - location;
        XIndex left = location;
        if (left < right) {
            //移动前半部分, 前翻
            if (table->offset >= length) {
                table->offset -= length;
            } else {
                table->offset = table->offset + table->capacity - length;
            }
            table->count += length;

            for (XIndex idx=0; idx<left; idx++) {
                __XCCircularBufferPageTableSet(table, idx, __XCCircularBufferPageTableGet(table, idx+length));
            }
            for (XIndex idx=left; idx<left+length; idx++) {
                __XCCircularBufferPageTableSet(table, idx, NULL);
            }
        } else {
            //移动后半部分, 后翻
            XIndex oldCount = table->count;
            table->count += length;

            for (XIndex offset=1; offset<=right; offset++) {
                __XCCircularBufferPageTableSet(table, table->count - offset, __XCCircularBufferPageTableGet(table, oldCount - offset));
            }
            for (XIndex idx=left; idx<left+length; idx++) {
                __XCCircularBufferPageTableSet(table, idx, NULL);
            }
        }
        return NULL;
    }
}

//0 < length <= table->count
//0 < length + location <= table->count
static inline XCCircularBufferPageTable_s * _Nullable __XCCircularBufferPageTableRemove(XCCircularBufferPageTable_s * _Nonnull table, XIndex location, XIndex length) {
    XIndex count = table->count - length;
    XIndex capacity = __XCCircularBufferGoodCapacity(count);
    if (capacity < table->capacity) {
        //缩容
        XCCircularBufferPageTable_s * result = __XCCircularBufferPageTableCreate(capacity);
        for (XIndex idx=0; idx<location; idx++) {
            __XCCircularBufferPageTableAdd(result, __XCCircularBufferPageTableGet(table, idx));
        }
        for (XIndex idx=location+length; idx<table->count; idx++) {
            __XCCircularBufferPageTableAdd(result, __XCCircularBufferPageTableGet(table, idx));
        }
        return result;
    } else {
        XIndex right = table->count - location - length;
        XIndex left = location;
        if (left < right) {
            //移动前半部分, 后翻 location-offset => location + length - offset
            for (XIndex offset=1; offset<=left; offset++) {
                __XCCircularBufferPageTableSet(table, location + length - offset, __XCCircularBufferPageTableGet(table, location-offset));
            }
            table->offset = (table->offset + length) % table->capacity;
            table->count -= length;
        } else {
            //移动后半部分, 前翻 location+length+offset => location+offset
            for (XIndex offset=0; offset<=right; offset++) {
                __XCCircularBufferPageTableSet(table, location+offset, __XCCircularBufferPageTableGet(table, location+length+offset));
            }
            table->count -= length;
        }
        return NULL;
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

static inline XCCircularBufferIndex __XCCircularBufferLoactionOfIndex(XCCircularBuffer_s * _Nonnull buffer, XIndex index) {
    XIndex r = buffer->_capacity - buffer->_offset;
    XIndex location = 0;
    if (index >= r) {
        location = index - r;
    } else {
        location = index + buffer->_offset;
    }
    return XCCircularBufferIndexMakeWithIndex(location);
}

static inline XIndex __XCCircularBufferIndexOfLoaction(XCCircularBuffer_s * _Nonnull buffer, XCCircularBufferIndex location) {
    XIndex loc = XCCircularBufferIndexToIndex(location);
    if (loc >= buffer->_offset) {
        return loc - buffer->_offset;
    } else {
        return buffer->_capacity - (buffer->_offset - loc);
    }
}


static inline void __XCCircularBufferEnumerate(XCCircularBuffer_s * _Nonnull buffer, XRange range, void * _Nullable context, XCArrayEnumerateCallBack_f _Nonnull func) {
    
    
}
static inline void _XCCircularBufferEnumerate(XCArrayPtr * _Nonnull array, XRange range, void * _Nullable context, XCArrayEnumerateCallBack_f _Nonnull func) {
    
    
}


//在location位置， 插入length

//0 < length
//0 <= location <= table->_base.count
static inline void __XCCircularBufferResizeByInsert(XCCircularBuffer_s * _Nonnull buffer, XIndex location, XIndex length) {
    
    XIndex capacity = __XCCircularBufferGoodCapacity(buffer->_base.count + length);
    
    //优先处理 1维 => 1维 1维 => 2维
    if (capacity > buffer->_capacity) {
        //拓容
        if (buffer->_capacity <= X_BUILD_CircularBufferPageCapacity && capacity > X_BUILD_CircularBufferPageCapacity) {
            //1维 => 2维
            
            return;
        } else if (capacity <= X_BUILD_CircularBufferPageCapacity) {
            //1维 => 1维
            
            return;
        } else {
            //2维 => 2维
            
            if (length % X_BUILD_CircularBufferPageCapacity == 0) {
                //length 是整数页
                
                //找到location所在的页
                
                
                //页拆分

                
                //插入页
                
                
                return;
            } else {
                //拓容
                
                
                //继续 进入移动
            }
        }
    }
    
    //移动
    if (length % X_BUILD_CircularBufferPageCapacity == 0) {
        if (location < buffer->_base.count - location) {
            //移动前半部分
            
        } else {
            //移动后半部分

        }
    } else {
        if (location < buffer->_base.count - location) {
            //移动前半部分
            
        } else {
            //移动后半部分

        }
    }
}


//从location开始删除length个

//0 < length <= table->_base.count
//0 < length + location <= table->_base.count
static inline void __XCCircularBufferResizeByRemove(XCCircularBuffer_s * _Nonnull buffer, XIndex location, XIndex length) {
    
    
    //降维

    
    
    if (length % X_BUILD_CircularBufferPageCapacity == 0) {
        
        
        
    } else {

        
    }
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
    XIndex capacity = buffer->_capacity;

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







