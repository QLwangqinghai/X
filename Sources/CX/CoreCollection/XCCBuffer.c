//
//  XCCBuffer.c
//  X
//
//  Created by vector on 2020/6/8.
//  Copyright © 2020 haoqi. All rights reserved.
//

#include "XCCBuffer.h"
#include "XMemory.h"
#include "XCCUtil.h"



XCCBuffer_s * _Nonnull XCCBufferCreate(XIndex elementSize, XIndex count) {
    XAssert(elementSize > 0, __func__, "");
    XAssert(count <= X_BUILD_ArrayCapacityMax, __func__, "");
    XSize size = sizeof(XCCBuffer_s) + elementSize * count;
    XCCBuffer_s * buffer = (XCCBuffer_s *)XAllocate(size);
    assert(buffer);
    XCCBaseInit(&buffer->_base, 0, elementSize, count);
    memset(&(buffer->_storage[0]), 0, elementSize * count);
    return buffer;
}

static inline void XCCBufferDestroy(XCCBuffer_s * _Nonnull buffer) {
    XAssert(NULL != buffer, __func__, "");
    free(buffer);
}


XPtr _Nonnull XCCBufferGetValue(XCCBuffer_s * _Nonnull buffer, XIndex location) {
    return &(buffer->_storage[buffer->_base.elementSize * location]);
}
void XCCBufferSetValue(XCCBuffer_s * _Nonnull buffer, XIndex location, XPtr _Nonnull values) {
    XUInt8 * ptr = &(buffer->_storage[buffer->_base.elementSize * location]);
    memcpy(ptr, values, buffer->_base.elementSize);
}

XPtr _Nonnull XCCBufferGetValues(XCCBuffer_s * _Nonnull buffer, XRange range) {
    return &(buffer->_storage[buffer->_base.elementSize * range.location]);
}
void XCCBufferSetValues(XCCBuffer_s * _Nonnull buffer, XRange range, XPtr _Nonnull values) {
    XUInt8 * ptr = &(buffer->_storage[buffer->_base.elementSize * range.location]);
    memcpy(ptr, values, buffer->_base.elementSize * range.length);
}


static inline void __XCCBufferEnumerate(XCCBuffer_s * _Nonnull buffer, XRange range, void * _Nullable context, XCCArrayEnumerateCallBack_f _Nonnull func) {
    
    
}
static inline void __XCCBufferReverseEnumerate(XCCBuffer_s * _Nonnull buffer, XRange range, void * _Nullable context, XCCArrayEnumerateCallBack_f _Nonnull func) {
    
    
}

#pragma mark - public api

void _XCCBufferEnumerate(XCCArrayPtr _Nonnull array, XRange range, void * _Nullable context, XCCArrayEnumerateCallBack_f _Nonnull func) {
    XCCBuffer_s * buffer = array;
    __XCCBufferEnumerate(buffer, range, context, func);
}

void _XCCBufferReverseEnumerate(XCCArrayPtr _Nonnull array, XRange range, void * _Nullable context, XCCArrayEnumerateCallBack_f _Nonnull func) {
    XCCBuffer_s * buffer = array;
    __XCCBufferReverseEnumerate(buffer, range, context, func);
}
