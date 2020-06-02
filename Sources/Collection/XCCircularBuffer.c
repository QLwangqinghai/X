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
 8-3
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
        if (location < table->count - location) {
            //移动前半部分
            if (table->offset >= location) {
                table->offset -= location;
            } else {
                table->offset = table->offset + table->capacity - length;
            }
            table->count += length;

            for (XIndex idx=0; idx<location; idx++) {
                __XCCircularBufferPageTableSet(table, idx, __XCCircularBufferPageTableGet(table, idx+length));
            }
        } else {
            //移动后半部分

            XIndex oldCount = table->count;
            XIndex moveCount = table->count - location;
            table->count += length;

            for (XIndex idx=oldCount-1; idx<moveCount; idx++) {
                __XCCircularBufferPageTableSet(table, idx, __XCCircularBufferPageTableGet(table, idx+location));
            }
        }
        
        return NULL;
    }
    
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
    
}
static inline void __XCCircularBufferPageTableRemove(XCCircularBufferPageTable_s * _Nonnull table, XIndex location, XIndex length) {
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


//static inline XCCircularBufferPage_s * _Nonnull _XCCircularBufferPageCreate(XIndex count) {
//    return XAlignedAllocate(sizeof(XCCircularBufferPage_s), 64);
//}
//
//static inline XCCircularBufferPageTable_s * _Nonnull _XCCircularBufferPageTableCreate(void) {
//    return XAlignedAllocate(sizeof(XCCircularBufferPageTable_s), 64);
//}
//
//#if CX_TARGET_RT_64_BIT
//static inline XCCircularBufferPageTableList_s * _Nonnull _XCCircularBufferPageTableListCreate(void) {
//    return XAlignedAllocate(sizeof(XCCircularBufferPageTableList_s), 64);
//}
//#endif



static inline XCCircularBufferLocation __XCCircularBufferLoactionOfIndex(XCCircularBuffer_s * _Nonnull buffer, XIndex index) {
    XIndex r = buffer->_capacity - buffer->_offset;
    XIndex location = 0;
    if (index >= r) {
        location = index - r;
    } else {
        location = index + buffer->_offset;
    }
    return XCCircularBufferLocationMakeWithIndex(location);
}

static inline XIndex __XCCircularBufferIndexOfLoaction(XCCircularBuffer_s * _Nonnull buffer, XCCircularBufferLocation location) {
    XIndex loc = XCCircularBufferLocationToIndex(location);
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

static inline void __XCCircularBufferResizeByInsert(XCCircularBuffer_s * _Nonnull buffer, XIndex location, XIndex length) {
    if (location < buffer->_base.count - location) {
        //移动前半部分
        
    } else {
        //移动后半部分

    }
}


static inline void __XCCircularBufferResizeByRemove(XCCircularBuffer_s * _Nonnull buffer, XIndex location, XIndex length) {

    
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
    
    
    if (range.length > 0) {
        //replace
        
    } else {
        if (itemsCount > 0) {
            //remove + insert

        } else {
            //remove
        }
    }
    
    
    if (itemsCount > 0) {
        assert(items);
    } else {
        if (autoResize) {
            //尝试缩容
            if (buffer->_count > bufferElementCapacity) {

            } else {
                size_t size = buffer->_count * elementSize;
                int itr = 0;
                CCIndex capacity = 0;
                for (itr = 0; itr<__CCCircularBufferSizeCount; itr++) {
                    if (size <= __CCCircularBufferSizes[itr]) {
                        size = __CCCircularBufferSizes[itr];
                        capacity = __CCCircularBufferSizes[itr] / buffer->_elementSize;
                        break;
                    }
                }
                if (buffer->_capacity < buffer->_bufferElementCapacity) {
                    if (capacity != 0) {
                        uint8_t * old = buffer->_storage;
                        assert(old);
                        uint8_t * ptr = CCAllocate(size);
                        if (buffer->_capacity <= bufferElementCapacity) {
                            CCIndex rLength = buffer->_capacity - buffer->_indexOffset;
                            if (rLength > buffer->_count) {
                                rLength = buffer->_count;
                            }
                            size_t rSize = elementSize * rLength;
                            memcpy(ptr, old + buffer->_indexOffset * elementSize, rSize);
                            if (rLength < buffer->_count) {
                                memcpy(ptr + rSize, old, elementSize * (buffer->_count - rLength));
                            }
                            __CCCircularBufferResetStorage(buffer, ptr, capacity, 0);
                            CCDeallocate(old);
                        } else {
                            
                            
                        }
                    }
                    
                    
                } else {
                    
                    
                }
                void * old = buffer->_storage;
                assert(old);
                uint8_t * ptr = CCAllocate(size);

   
                assert(itr != __CCCircularBufferSizeCount);
                if (capacity != buffer->_capacity) {
                    assert(capacity < buffer->_capacity);
                    
                    if (buffer->_capacity < buffer->_bufferElementCapacity) {
                        if (capacity != 0) {
                            uint8_t * old = buffer->_storage;
                            assert(old);
                            uint8_t * ptr = CCAllocate(size);
                            if (buffer->_capacity <= bufferElementCapacity) {
                                CCIndex rLength = buffer->_capacity - buffer->_indexOffset;
                                if (rLength > buffer->_count) {
                                    rLength = buffer->_count;
                                }
                                size_t rSize = elementSize * rLength;
                                memcpy(ptr, old + buffer->_indexOffset * elementSize, rSize);
                                if (rLength < buffer->_count) {
                                    memcpy(ptr + rSize, old, elementSize * (buffer->_count - rLength));
                                }
                                __CCCircularBufferResetStorage(buffer, ptr, capacity, 0);
                                CCDeallocate(old);
                            } else {
                                
                                
                            }
                        }
                        
                        
                    } else {
                        
                        
                    }
                    
                    //缩容
                    if (capacity == 0) {
                        //释放全部空间
                        
                    } else {
                        if (buffer->_capacity < buffer->_bufferElementCapacity) {
                            
                            
                            
                            
                        } else {
                            
                            
                        }
                        uint8_t * ptr = CCAllocate(size);
                        CCIndex idx = __CCCircularBufferLoactionToIndex(buffer, 0);
                        
                        memcpy(ptr, <#const void *__src#>, <#size_t __n#>)
                        
                        
                        
                        
                        
                    }
                }
            }
        }
        return;
    }
    
    CCIndex offset = 0;
    CCRange range = {};//最大的不动的区间
    CCIndex removedCount = 0;
    CCIndex insertedCount = 0;
    
    for (CCIndex i=0; i<itemsCount; i++) {
        CCRangeReplace item = items[i];
        assert(item.range.location >= offset);
        CCIndex length = range.location - offset;
        if (length > range.length) {
            range.location = offset;
            range.length = length;
        }
        offset = item.range.location + item.range.length;
        if (item.newLength > 0) {
            assert(item.newValues);
        }
        removedCount += item.range.length;
        insertedCount += item.newLength;
    }
    assert(offset <= buffer->_count);
    
    CCIndex newCount = buffer->_count - removedCount + insertedCount;
    assert(newCount <= buffer->_capacity && !resize);
    do {
        CCRangeReplace item = items[itemsCount-1];
        CCIndex end = item.range.location + item.range.length;
        if (end < buffer->_count) {
            CCIndex length = buffer->_count - end;
            if (length > range.length) {
                range.location = end;
                range.length = length;
            }
        }
    } while (false);
    
    if (buffer->_count == 0) {
        buffer->_indexOffset = 0;
    }
//
//    //        CCIndex loc = __CCCircularBufferIndexToLoaction(<#CCCircularBuffer_s * _Nonnull buffer#>, <#CCIndex index#>)
//
//    else if (newCount == 0) {
//        //remove all
//
//
//    } else {
//
//    }
//
    if (onRemove) {
        for (CCIndex i=0; i<itemsCount; i++) {
            CCRangeReplace item = items[i];
            
            
            
        }
    }
    
    uint32_t elementSize = buffer->_elementSize;
    uint64_t count = 0;
    for (int32_t vi=0; vi<newVectorCount; vi++) {
        CCVector_s v = newVectors[vi];
        count += v.count;
    }
    
    assert(count <= CCCountLimit);
    uint32_t newCount = (uint32_t)count;

    uint32_t capacity = buffer->_count - range.length + newCount;
    if (capacity > buffer->_capacity) {//拓容
        uint32_t realCapacity = __CCCircularBufferGoodCapacity(capacity);
        assert(realCapacity <= CCCountLimit);
        assert(capacity <= CCCountLimit);
        uint64_t elementsSize = (uint64_t)elementSize * (uint64_t)realCapacity;
        assert(elementSize <= CCCircularBufferElementsSizeLimit);
        
        CCCircularBuffer_s * result = CCAllocate(elementsSize + sizeof(CCCircularBuffer_s));
        
        result->_capacity = realCapacity;
        result->_elementSize = elementSize;
        result->_indexOffset = 0;
        result->_count = capacity;
        
        CCVector_s tVec[2] = {};
        int32_t tVecCount = __CCCircularBufferGetItemsInRange(buffer, CCRangeMake(0, range.location), tVec);
        uint32_t index = 0;
        for (int32_t vi=0; vi<tVecCount; vi++) {
            CCVector_s v = tVec[vi];
            ___CCCircularBufferSetValues(result, index, v);
            index += v.count;
        }
        
        for (int32_t vi=0; vi<newVectorCount; vi++) {
            CCVector_s v = newVectors[vi];
            ___CCCircularBufferSetValues(result, index, v);
            index += v.count;
        }
        
        tVecCount = __CCCircularBufferGetItemsInRange(buffer, CCRangeMake(range.location + newCount, capacity - range.location - newCount), tVec);
        for (int32_t vi=0; vi<tVecCount; vi++) {
            CCVector_s v = tVec[vi];
            ___CCCircularBufferSetValues(result, index, v);
            index += v.count;
        }
        
        CCDeallocate(buffer);
        return result;
    } else {
        if (range.length == newCount) {
            uint32_t index = range.location;
            for (int32_t vi=0; vi<newVectorCount; vi++) {
                CCVector_s v = newVectors[vi];
                ___CCCircularBufferSetValues(buffer, index, v);
                index += v.count;
            }
        } else {
            uint32_t rightCount = buffer->_count - range.location - range.length;
            int32_t countChanged = (int32_t)newCount - (int32_t)range.length;
            if (range.location < rightCount) {//移动首部
                int32_t offset = - countChanged;
                if (offset < 0) {//前移
                    ___CCCircularBufferMoveIndexOffset(buffer, offset);
                    buffer->_count = capacity;
                    ___CCCircularBufferMoveValues(buffer, CCRangeMake(countChanged, range.location), offset);
                } else {//后移
                    ___CCCircularBufferMoveValues(buffer, CCRangeMake(0, range.location), offset);
                    ___CCCircularBufferMoveIndexOffset(buffer, offset);
                    buffer->_count = capacity;
                }
            } else {//移动尾部
                int32_t offset = countChanged;
                if (offset < 0) {//前移
                    ___CCCircularBufferMoveValues(buffer, CCRangeMake(range.location + range.length, rightCount), offset);
                    buffer->_count = capacity;
                } else {//后移
                    buffer->_count = capacity;
                    ___CCCircularBufferMoveValues(buffer, CCRangeMake(range.location + range.length, rightCount), offset);
                }
            }
            uint32_t index = range.location;
            for (int32_t vi=0; vi<newVectorCount; vi++) {
                CCVector_s v = newVectors[vi];
                ___CCCircularBufferSetValues(buffer, index, v);
                index += v.count;
            }
        }
        return buffer;
    }
}







