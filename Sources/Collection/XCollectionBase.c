//
//  XCollection.c
//  X
//
//  Created by clf on 2020/6/1.
//  Copyright © 2020 haoqi. All rights reserved.
//

#include "XCollectionBase.h"


static inline XCArray_s * _XCArrayUnpack(XCArrayPtr _Nonnull ptr, const char * func) {
    XAssert(NULL != ptr, func, "");
    XCArray_s * array = (XCArray_s *)ptr;
 
    return array;
}

XIndex XCArrayGetCount(XCArrayPtr _Nonnull ptr) {
    XCArray_s * array = _XCArrayUnpack(ptr, __func__);
    return array->count;
}
XIndex XCArrayGetElementSize(XCArrayPtr _Nonnull ptr) {
    XCArray_s * array = _XCArrayUnpack(ptr, __func__);
    return array->elementSize;
}



void XCArrayEnumerate(XCArrayPtr _Nonnull ptr, XRange range, void * _Nullable context, XCArrayEnumerateCallBack_f _Nonnull func) {
    XCArray_s * array = _XCArrayUnpack(ptr, __func__);

//    assert(buffer);
//    assert(func);
//    if (range.length == 0) {
//        return;
//    }
//    assert(range.location + range.length >= range.location);
//    assert(range.location + range.length <= buffer->_count);
//
//    size_t elementSize = buffer->_elementSize;
//    CCIndex bufferElementCapacity = buffer->_bufferElementCapacity;
//    
//    CCRange remain = range;
//    
//    if (buffer->_capacity > buffer->_bufferElementCapacity) {
//        CCPageTablePtr table = (CCPageTablePtr)(buffer->_storage);
//
//        CCIndex pageIndex = 0;
//        
//        do {
//            CCIndex location = __CCCircularBufferIndexToLoaction(buffer, remain.location);
//            CCCircularBufferLocationPath path = __CCCircularBufferGetLocationPath(buffer, location);
//            pageIndex = path.pageIndex;
//
//            uint8_t * storage = __CCCircularBufferGetPage(buffer, pageIndex);
//            CCIndex rLength = bufferElementCapacity - path.index;
//            if (rLength > remain.length) {
//                rLength = remain.length;
//            }
//            assert(rLength > 0);
//            func(context, CCRangeMake(remain.location, rLength), elementSize, storage + buffer->_indexOffset * elementSize);
//            remain.location += rLength;
//            remain.length -= rLength;
//            pageIndex += 1;
//            if (pageIndex >= table->_count) {
//                pageIndex = 0;
//            }
//        } while (0);
//        while (remain.length > 0) {
//            uint8_t * storage = __CCCircularBufferGetPage(buffer, pageIndex);
//            CCIndex rLength = bufferElementCapacity;
//            if (rLength > remain.length) {
//                rLength = remain.length;
//            }
//            assert(rLength > 0);
//            func(context, CCRangeMake(remain.location, rLength), elementSize, storage + buffer->_indexOffset * elementSize);
//            remain.location += rLength;
//            remain.length -= rLength;
//            pageIndex += 1;
//            if (pageIndex >= table->_count) {
//                pageIndex = 0;
//            }
//        }
//    } else {
//        CCIndex location = __CCCircularBufferIndexToLoaction(buffer, remain.location);
//        CCCircularBufferLocationPath path = __CCCircularBufferGetLocationPath(buffer, location);
//        uint8_t * storage = __CCCircularBufferGetPage(buffer, path.pageIndex);
//        CCIndex rLength = buffer->_capacity - location;
//        if (rLength > range.length) {
//            rLength = range.length;
//        }
//        assert(rLength > 0);
//        func(context, CCRangeMake(remain.location, rLength), elementSize, storage + location * elementSize);
//        remain.location += rLength;
//        remain.length -= rLength;
//        if (rLength < range.length) {
//            func(context, remain, elementSize, storage);
//        }
//    }
}

