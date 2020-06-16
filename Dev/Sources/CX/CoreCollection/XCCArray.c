//
//  XCCArray.c
//  X
//
//  Created by vector on 2020/6/8.
//  Copyright © 2020 haoqi. All rights reserved.
//

#include "XCCArray.h"




#pragma mark - base


static inline XCCArray_s * _XCArrayUnpack(XCCArrayPtr _Nonnull ptr, const char * func) {
    XAssert(NULL != ptr, func, "");
    XCCArray_s * array = (XCCArray_s *)ptr;
 
    return array;
}

XIndex XCArrayGetCount(XCCArrayPtr _Nonnull ptr) {
    XCCArray_s * array = _XCArrayUnpack(ptr, __func__);
    return array->count;
}
XIndex XCArrayGetElementSize(XCCArrayPtr _Nonnull ptr) {
    XCCArray_s * array = _XCArrayUnpack(ptr, __func__);
    return array->elementSize;
}

void XCArrayReverseEnumerate(XCCArrayPtr _Nonnull array, XRange range, void * _Nullable context, XCCArrayEnumerateCallBack_f _Nonnull func) {
    
}


void XCArrayEnumerate(XCCArrayPtr _Nonnull ptr, XRange range, void * _Nullable context, XCCArrayEnumerateCallBack_f _Nonnull func) {
    XCCArray_s * array = _XCArrayUnpack(ptr, __func__);

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

