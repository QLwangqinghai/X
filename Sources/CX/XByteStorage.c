//
//  XByteStorage.c
//  
//
//  Created by vector on 2020/4/3.
//

#include "XType.h"
#include "internal/XRuntimeInternal.h"
#include "internal/XAllocator.h"
#include "XMemory.h"
#include "internal/XClass.h"
#include "internal/XLock.h"
#include <pthread.h>

_Static_assert(sizeof(XSpinlock_t) == sizeof(XUInt32), "error");


static XSpinlockTable_s * _Nullable __XByteStorageLockTableShared = NULL;
void __XByteStorageLockTableOnceBlockFunc(void) {
    __XByteStorageLockTableShared = XSpinlockTableCreate(499);
}
XSpinlockTable_s * _Nonnull _XByteStorageLockTableShared(void) {
    static pthread_once_t token = PTHREAD_ONCE_INIT;
    pthread_once(&token, &__XByteStorageLockTableOnceBlockFunc);
    assert(__XByteStorageLockTableShared);
    return __XByteStorageLockTableShared;
}

void _XByteStorageGetRetainedBuffer(_XByteStorage * _Nonnull storage) {
    XSpinlockTable_s * table = _XByteStorageLockTableShared();
    XIndex h = XAddressHash(storage);

    XSpinlock_t * lock = XSpinlockTableGet(table, h % table->capacity);
    XSpinlockLock(lock);
}

void _XByteStorageLock(_XByteStorage * _Nonnull storage) {
    XSpinlockTable_s * table = _XByteStorageLockTableShared();
    XIndex h = XAddressHash(storage);

    XSpinlock_t * lock = XSpinlockTableGet(table, h % table->capacity);
    XSpinlockLock(lock);
}
void _XByteStorageUnlock(_XByteStorage * _Nonnull storage) {
    
}

#pragma mark - base

#if CX_TARGET_RT_64_BIT
    #define XByteStorageHashNoneFlag 0x8000000000000000ULL
    #define XByteStorageHashMask 0x7FFFFFFFFFFFFFFFULL
#else
    #define XByteStorageHashNoneFlag 0x80000000UL
    #define XByteStorageHashMask 0x7FFFFFFFUL
#endif

#define XByteStorageHashCodeIsValid(h) (((h) & XByteStorageHashNoneFlag) != XByteStorageHashNoneFlag)


#pragma mark - _XBuffer

#define _XBufferClearWhenDeallocFlag X_BUILD_UInt(1)
#define _XBufferRcOne X_BUILD_UInt(2)
#define _XBufferRcMax (XFastUIntMax-X_BUILD_UInt(1))

_XBuffer * _Nonnull _XBufferAllocate(XPtr _Nonnull content, XUInt size, XBool clearWhenDealloc) {
    XAssert(size <= (XUIntMax - X_BUILD_UInt(0x10)), __func__, "size to large");
    XUInt bufferSize = (size + 15) & (~X_BUILD_UInt(0xf));
    _XBuffer * buffer = XAlignedAllocate(bufferSize + sizeof(_XBuffer), 16);
    buffer->size = bufferSize;
    _Atomic(XFastUInt) * rcPtr = &(buffer->_refCount);
    XFastUInt rc = _XBufferRcOne;
    if (clearWhenDealloc) {
        rc |= _XBufferClearWhenDeallocFlag;
    }
    atomic_store(rcPtr, rc);
    memcpy(&(buffer->content[0]), content, size);
    
    if (bufferSize > size) {
        bzero(&(buffer->content[size]), bufferSize - size);
    }
    return buffer;
}

void __XBufferDeallocate(_XBuffer * _Nonnull buffer) {
    XDeallocate(buffer);
}

XPtr _Nonnull _XBufferGetContent(_XBuffer * _Nonnull buffer) {
    assert(buffer);
    return &(buffer->content);
}

void __XBufferClear(_XBuffer * _Nonnull buffer) {

}


void _XBufferSetClearWhenDealloc(_XBuffer * _Nonnull buffer) {
    assert(buffer);
    _Atomic(XFastUInt) * rcInfoPtr = &(buffer->_refCount);

    XFastUInt rcInfo = 0;
    XFastUInt newRcInfo = 0;
    
    do {
        rcInfo = atomic_load(rcInfoPtr);
        if ((rcInfo & _XBufferClearWhenDeallocFlag) == _XBufferClearWhenDeallocFlag) {
            return;
        }
        assert(rcInfo >= _XBufferRcOne);
        newRcInfo = rcInfo | _XBufferClearWhenDeallocFlag;
    } while (!atomic_compare_exchange_strong(rcInfoPtr, &rcInfo, newRcInfo));
    return;
}


_XBuffer * _Nonnull _XBufferRetain(_XBuffer * _Nonnull buffer) {
    assert(buffer);
    _Atomic(XFastUInt) * rcInfoPtr = &(buffer->_refCount);

    XFastUInt rcInfo = 0;
    XFastUInt newRcInfo = 0;
    
    do {
        rcInfo = atomic_load(rcInfoPtr);
        if (rcInfo >= _XBufferRcMax) {
            return buffer;
        }
        assert(rcInfo >= _XBufferRcOne);
        newRcInfo = rcInfo + _XBufferRcOne;
    } while (!atomic_compare_exchange_strong(rcInfoPtr, &rcInfo, newRcInfo));

    return buffer;
}

void _XBufferRelease(_XBuffer * _Nonnull buffer) {
    assert(buffer);
    _Atomic(XFastUInt) * rcInfoPtr = &(buffer->_refCount);

    XFastUInt rcInfo = 0;
    XFastUInt newRcInfo = 0;

    do {
        rcInfo = atomic_load(rcInfoPtr);
        if (rcInfo >= _XBufferRcMax) {
            return;
        }
        assert(rcInfo >= _XBufferRcOne);
        newRcInfo = rcInfo - _XBufferRcOne;
    } while (!atomic_compare_exchange_strong(rcInfoPtr, &rcInfo, newRcInfo));
    if (newRcInfo < _XBufferRcOne) {
        if ((newRcInfo & _XBufferClearWhenDeallocFlag) == _XBufferClearWhenDeallocFlag) {
            __XBufferClear(buffer);
        }
        __XBufferDeallocate(buffer);
    }
}

#pragma mark - _XByteStorage

/*
 TaggedObject32

 refType: 1, value = 1
 taggedContent: {
    class: 2
    objectContent: 28 {
        len: 4
        content: 24
    }
 }
 flag: 1, value = 1
*/
/*
 TaggedObject64

 refType: 1, value = 1
 taggedContent: 62 {
    class: 2
    objectContent: 60 {
       len: 4
       content: 56
   }
 }
 flag: 1, value = 1
*/

#if CX_TARGET_RT_64_BIT
#define X_BUILD_TaggedByteStorageContentMask 0xFFFFFFFFFFFFFFULL
#define X_BUILD_TaggedByteStorageContentLengthMask 0x7ULL
#define X_BUILD_TaggedByteStorageContentLengthShift 57ULL
#define X_BUILD_TaggedByteStorageContentLengthMax 0x7UL
#else
#define X_BUILD_TaggedByteStorageContentMask 0xFFFFFFUL
#define X_BUILD_TaggedByteStorageContentLengthMask 0x7UL
#define X_BUILD_TaggedByteStorageContentLengthShift 25ULL
#define X_BUILD_TaggedByteStorageContentLengthMax 0x3UL
#endif


static inline XUInt _XByteStorageGoodInlineLength(XSize length) {
#if CX_TARGET_RT_64_BIT
        XUInt bufferSize = 16;
        if (length > 16) {
            bufferSize = (length + X_BUILD_UInt(0x7)) & (~X_BUILD_UInt(0x7));
        }
#else
        XUInt bufferSize = 8;
        if (length > 8) {
            bufferSize = (length + X_BUILD_UInt(0x3)) & (~X_BUILD_UInt(0x3));
        }
#endif
    return bufferSize;
}

static void _XByteStorageDistory(_XByteStorage * _Nonnull storage) {
    if (0 == storage->content.flag) {
        XSize length = storage->content.load;
        XUInt bufferSize = _XByteStorageGoodInlineLength(length);
        bufferSize += sizeof(XUInt);
        bzero(&(storage->content), bufferSize);
    } else {
        XUInt address = storage->content.load;
        address = address << 1;
        _XBuffer * buffer = (_XBuffer *)((uintptr_t)address);
        _XBufferRelease(buffer);
    }
}

static XRef _Nonnull _XByteStorageCreate(XBool isString, XObjectFlag flag, const XUInt8 * _Nullable buffer, XSize length, const char * _Nonnull func) {
    if (length > 0) {
        XAssert(NULL != buffer, func, "length > 0, buffer cannot NULL");
    }
    if (length <= X_BUILD_TaggedByteStorageContentLengthMax) {
        XUInt bytes = 0;
        memcpy(&bytes, buffer, length);
        
#if CX_TARGET_RT_BIG_ENDIAN
        bytes = bytes >> X_BUILD_UInt(8);
#endif
        XUInt len = length;
        len = len << X_BUILD_TaggedByteStorageContentLengthShift;
        XUInt t = isString ? X_BUILD_TaggedObjectClassString : X_BUILD_TaggedObjectClassData;
        XUInt v = X_BUILD_TaggedObjectFlag | t | len | (bytes << X_BUILD_UInt(1));
        /*
         abc00000
         */
        /*
         大端
         xacb0000
         */
        /*
         小端
         0000bca
         */
        return (XRef)((uintptr_t)v);
    }

    const XType_s * cls = isString ? _XClassOf(String) : _XClassOf(Data);
    const _XAllocator_s * allocator = &_XValueAllocator;
    XSize size = sizeof(_XByteStorageContentBase_t);
    
    XObjectRcFlag rcFlag = 0;
    if ((flag & XObjectFlagStatic) == XObjectFlagStatic) {
        rcFlag |= XObjectRcFlagStatic;
    }
    
    if (length >= _XByteStorageContentBufferSizeMin) {
        XRef ref = allocator->allocateRef(allocator, (XClass)cls, size, rcFlag);
        _XByteStorage * storage = (_XByteStorage *)ref;
        _XBuffer * buf = _XBufferAllocate((XPtr)buffer, length, ((flag & XObjectFlagClearWhenDealloc) == XObjectFlagClearWhenDealloc));
        XUInt address = (XUInt)((uintptr_t)buf);
        address = address >> 1;
        storage->content.flag = 1;
        storage->content.load = address;
        return ref;
    } else {
        XUInt bufferSize = _XByteStorageGoodInlineLength(length);
        size += bufferSize;
        XRef ref = allocator->allocateRef(allocator, (XClass)cls, size, rcFlag);
        _XByteStorage * storage = (_XByteStorage *)ref;

        _XByteStorageContentBase_t * storageContent = (_XByteStorageContentBase_t *)&(storage->content);
        storageContent->flag = 0;
        storageContent->load = length;
        if (length > 0) {
            memcpy(&(storage->extended[0]), buffer, length);
        }

        if (bufferSize > length) {
            bzero(&(storage->extended[length]), bufferSize - length);
        }
        return ref;
    }
}

static _XByteStorage * _Nonnull _XByteStorageCreateWithBuffer(XBool isString, XObjectFlag flag, _XBuffer * _Nonnull xbuffer, const char * _Nonnull func) {
    XAssert(NULL != xbuffer, func, "length > 0, buffer cannot NULL");
    
    const XType_s * cls = isString ? _XClassOf(String) : _XClassOf(Data);
    const _XAllocator_s * allocator = &_XValueAllocator;
    XSize size = sizeof(_XByteStorageContentBase_t);
    
    XObjectRcFlag rcFlag = 0;
    if ((flag & XObjectFlagStatic) == XObjectFlagStatic) {
        rcFlag |= XObjectRcFlagStatic;
    }
    
    if (xbuffer->size >= _XByteStorageContentBufferSizeMin) {
        XRef ref = allocator->allocateRef(allocator, (XClass)cls, size, rcFlag);
        _XByteStorage * storage = (_XByteStorage *)ref;

        _XBuffer * buf = _XBufferRetain(xbuffer);
        if ((flag & XObjectFlagClearWhenDealloc) == XObjectFlagClearWhenDealloc) {
            _XBufferSetClearWhenDealloc(buf);
        }
        XUInt address = (XUInt)((uintptr_t)buf);
        address = address >> 1;
        storage->content.flag = 1;
        storage->content.load = address;
        return ref;
    } else {
        XUInt bufferSize = _XByteStorageGoodInlineLength(xbuffer->size);
        XRef ref = allocator->allocateRef(allocator, (XClass)cls, size, rcFlag);
        _XByteStorage * storage = (_XByteStorage *)ref;
        storage->content.flag = 0;
        storage->content.load = xbuffer->size;
        
        if (xbuffer->size > 0) {
            memcpy(&(storage->extended[0]), &(xbuffer->content[0]), xbuffer->size);
        }
        if (bufferSize > xbuffer->size) {
            bzero(&(storage->extended[xbuffer->size]), bufferSize - xbuffer->size);
        }
        return ref;
    }
}


static _XByteStorage * _Nonnull __XRefAsByteStorage(XRef _Nonnull ref, XBool * _Nonnull isString, const char * _Nonnull func, const char * _Nonnull desc) {
    XIndex typeId = _XHeapRefGetTypeId(ref);
    if (X_BUILD_TypeId_String == typeId) {
        *isString = true;
    } else if (X_BUILD_TypeId_Data == typeId) {
        *isString = false;
    } else {
        XAssert(false, func, desc);
    }
    return (_XByteStorage *)ref;
}

XByteStorageUnpacked_t _XByteStorageUnpack(XPtr _Nonnull ref, const char * _Nonnull func, const char * _Nonnull desc) {
    XByteStorageUnpacked_t result = {};
    XTaggedType type = _XRefGetTaggedType(ref);
    if (type > XTaggedTypeMax) {
        XBool isString = false;
        _XByteStorage * storage = __XRefAsByteStorage(ref, &isString, func, desc);
        result.isString = isString ? 1 : 0;
        if (1 == storage->content.flag) {
            XUInt address = storage->content.load;
            address = address << 1;
            _XBuffer * buffer = (_XBuffer *)((uintptr_t)address);
            result.content.large = buffer;
            result.contentType = 2;
            result.length = buffer->size;
        } else {
            result.length = storage->content.load;
            result.content.small = &(storage->extended[0]);
            result.contentType = 1;
        }
        return result;
    } else {
        if (XTaggedTypeByteStorage == type) {

        } else {
            XAssert(false, func, desc);
        }
        result.isString = ((X_BUILD_TaggedObjectByteStorageDataFlag & (XUInt)ref) == X_BUILD_TaggedObjectByteStorageDataFlag) ? 0 : 1;
        result.contentType = 0;

        XUInt v = (XUInt)((uintptr_t)ref);
        XUInt storageContent = (v >> 1) & X_BUILD_TaggedByteStorageContentMask;
        XUInt len = (v >> X_BUILD_TaggedByteStorageContentLengthShift) & X_BUILD_TaggedByteStorageContentLengthMask;
        XAssert(len <= X_BUILD_TaggedByteStorageContentLengthMax, func, "");
        
        XUInt bytes = storageContent;
        /*
         abc00000
         */
        /*
         大端
         xacb0000
         */
        /*
         小端
         0000bca
         */
        
#if CX_TARGET_RT_BIG_ENDIAN
        bytes = bytes << X_BUILD_UInt(8);
#endif
        
        result.content.__nano = bytes;
        return result;
    }
}

XHashCode _XByteStorageHashByte(XUInt8 * _Nullable bytes, XSize length, const char * _Nonnull func) {
    if (length > 0) {
        XAssert(NULL != bytes, func, "unknown error");
    }
    return XHash(bytes, length) & XByteStorageHashMask;
}

XHashCode _XByteStorageHash(XRef _Nonnull ref, XBool isString, const char * _Nonnull func) {
    XAssert(NULL != ref, func, "ref NULL error");
    const char * desc = isString ? "not String instance" : "not Data instance";
    XByteStorageUnpacked_t v = _XByteStorageUnpack(ref, func, desc);
    uint32_t isStringValue = isString ? 1 : 0;
    XAssert(isStringValue == v.isString, func, desc);
    if (0 == v.contentType) {
        return _XByteStorageHashByte(&(v.content.nano[0]), v.length, func);
    } else if (1 == v.contentType) {
        return _XByteStorageHashByte(v.content.small, v.length, func);
    } else if (2 == v.contentType) {
//        XFastUInt hashCode = atomic_load(&(v.content.large->hashCode));
//        if ((hashCode & XByteStorageHashNoneFlag) == XByteStorageHashNoneFlag) {
//            XUInt8 * bytes = &(v.content.large->buffer->content[v.content.large->offset]);
//            XUInt32 length = v.content.large->length;
//            hashCode = _XByteStorageHashByte(bytes, length, func);
//            atomic_store(&(v.content.large->hashCode), hashCode);
//        }
//        return hashCode;
        return 0;
    } else {
        XAssert(false, func, "unknown error");
    }
}
XUInt8 * _Nonnull _XByteStorageUnpackedGetByte(XByteStorageUnpacked_t * _Nonnull ptr, const char * _Nonnull func) {
    if (0 == ptr->contentType) {
        return &(ptr->content.nano[0]);
    } else if (1 == ptr->contentType) {
        return ptr->content.small;
    } else if (2 == ptr->contentType) {
        return &(ptr->content.large->content[0]);
    } else {
        XAssert(false, func, "unknown error");
    }
}


XComparisonResult _XByteStorageUnpackedCompare(XByteStorageUnpacked_t * _Nonnull lhs, XByteStorageUnpacked_t * _Nonnull rhs, const char * _Nonnull func) {
    XSize leftLength = lhs->length;
    XSize rightLength = rhs->length;
    XUInt8 * left = _XByteStorageUnpackedGetByte(lhs, func);
    XUInt8 * right = _XByteStorageUnpackedGetByte(rhs, func);
    
    XSize size = MIN(leftLength, rightLength);
    
    XComparisonResult result = XMemoryCompare(left, right, size);
    if (XCompareEqualTo == result) {
        if (leftLength < rightLength) {
            return XCompareLessThan;
        } else if (leftLength > rightLength) {
            return XCompareGreaterThan;
        }
    }
    return result;
}

XBool _XByteStorageUnpackedEqual(XByteStorageUnpacked_t * _Nonnull lhs, XByteStorageUnpacked_t * _Nonnull rhs, const char * _Nonnull func) {
    XSize leftLength = lhs->length;
    XSize rightLength = rhs->length;
    XUInt8 * left = _XByteStorageUnpackedGetByte(lhs, func);
    XUInt8 * right = _XByteStorageUnpackedGetByte(rhs, func);
    if (leftLength != rightLength) {
        return false;
    }
    XSize size = leftLength;
    
    XComparisonResult result = XMemoryCompare(left, right, size);
    return (XCompareEqualTo == result);
}


XComparisonResult _XByteStorageCompare(XRef _Nonnull lhs, XRef _Nonnull rhs, XBool isString, const char * _Nonnull func) {
    XAssert(NULL != lhs, func, "lhs NULL error");
    XAssert(NULL != rhs, func, "rhs NULL error");
    
    const char * desc = isString ? "not String instance" : "not Data instance";
    XByteStorageUnpacked_t left = _XByteStorageUnpack(lhs, func, desc);
    XByteStorageUnpacked_t right = _XByteStorageUnpack(rhs, func, desc);
    uint32_t isStringValue = isString ? 1 : 0;
    XAssert(isStringValue == left.isString, func, desc);
    XAssert(isStringValue == right.isString, func, desc);
    return _XByteStorageUnpackedCompare(&left, &right, func);
}



XBool _XByteStorageEqual(XRef _Nonnull lhs, XRef _Nonnull rhs, XBool isString, const char * _Nonnull func) {
    XAssert(NULL != lhs, func, "lhs NULL error");
    XAssert(NULL != rhs, func, "rhs NULL error");

    const char * desc = isString ? "not String instance" : "not Data instance";
    XByteStorageUnpacked_t left = _XByteStorageUnpack(lhs, func, desc);
    XByteStorageUnpacked_t right = _XByteStorageUnpack(rhs, func, desc);
    uint32_t isStringValue = isString ? 1 : 0;
    XAssert(isStringValue == left.isString, func, desc);
    XAssert(isStringValue == right.isString, func, desc);

    if (2 == left.contentType && 2 == right.contentType) {
        XFastUInt leftHashCode = atomic_load(&(left.content.large->hashCode));
        XFastUInt rightHashCode = atomic_load(&(right.content.large->hashCode));
        if (XByteStorageHashCodeIsValid(leftHashCode) && XByteStorageHashCodeIsValid(rightHashCode)) {
            if (leftHashCode != rightHashCode) {
                return false;
            }
        }
    }
    return _XByteStorageUnpackedEqual(&left, &right, func);
}


#pragma mark - Common

typedef XUInt32 XStringEncoding;

static const XStringEncoding XStringEncodingUtf8 = 1;


#pragma mark - XString

#define XStringEmpty ((XString)((uintptr_t)(X_BUILD_TaggedObjectFlag | X_BUILD_TaggedObjectClassString)))


static XRange _XRangeOfString(const XUInt8 * _Nonnull bytes, size_t length) {
    XIndex begin = 0;
    XIndex end = length;
    XIndex offset = 0;
    if (length >= 3) {
        //check bom
        if (0xEF == bytes[0] && 0xBB == bytes[1] && 0xBF == bytes[2]) {
            begin = 3;
            offset = 3;
        }
    }
    XIndex errorIndex = XIndexNotFound;
    while (length - offset > 0 && SIZE_T_MAX == errorIndex) {
        uint8_t byte = bytes[offset];
        if (byte == 0x0) {
            //终止符
            end = offset;
            break;
        } else if (byte < 0x80) {
            //1
            offset += 1;
        } else if ((byte & 0xE0) == 0xC0) {
            //2
            if (offset > length-2) {
                errorIndex = offset;
                break;
            }
            if ((byte & 0x1E) == 0) {
                //0x0080 ~ 0x07FF
                errorIndex = offset;
                break;
            }
            uint8_t tmp = bytes[offset+1];
            if ((tmp & 0xC0) == 0x80) {
                //10xxxxxx
                offset += 2;
            } else {
                errorIndex = offset;
            }
        } else if ((byte & 0xF0) == 0xE0) {
            //3
            if (offset > length-3) {
                errorIndex = offset;
                break;
            }
            
            uint16_t tmp = *(uint16_t *)(bytes + offset + 1);
            
#if CX_TARGET_RT_BIG_ENDIAN
            //大端
            if ((byte & 0xF) == 0 && (tmp & 0x2000) == 0) {
                //0x0800 ~ 0xFFFF
                errorIndex = offset;
                break;
            }
#else
            //小端
            if ((byte & 0xF) == 0 && (tmp & 0x20) == 0) {
                //0x0800 ~ 0xFFFF
                errorIndex = offset;
                break;
            }
#endif
            
            if ((tmp & 0xC0C0) == 0x8080) {
                //10xxxxxx 10xxxxxx
                offset += 3;
            } else {
                errorIndex = offset;
                break;
            }
        } else if ((byte & 0xF8) == 0xF0) {
            //4
            if (offset > length-4) {
                errorIndex = offset;
                break;
            }
            uint32_t tmp = *(uint32_t *)(bytes + offset);
            
#if CX_TARGET_RT_BIG_ENDIAN
            //大端
            if ((tmp & 0x7300000) == 0) {
                //0x10000 ~ 0x1FFFFF
                errorIndex = offset;
                break;
            }
            if ((tmp & 0xF8C0C0C0) == 0xF0808080) {
                offset += 4;
            } else {
                errorIndex = offset;
                break;
            }
#else
            //小端
            if ((tmp & 0x3007) == 0) {
                //0x10000 ~ 0x1FFFFF
                errorIndex = offset;
                break;
            }
            if ((tmp & 0xC0C0C0F8) == 0x808080F0) {
                offset += 4;
            } else {
                errorIndex = offset;
                break;
            }
#endif
        }
    }
    if (XIndexNotFound != errorIndex) {
        return XRangeMake(XIndexNotFound, 0);
    } else {
        return XRangeMake(begin, end - begin);
    }
}

XString _Nullable _XStringCreateWithUtf8Bytes(XObjectFlag flag, const XUInt8 * _Nonnull bytes, XSize length, const char * _Nonnull func) {
    XRange range = _XRangeOfString(bytes, length);
    XAssert(length < UINT32_MAX, func, "too large");
    if (XIndexNotFound != range.location) {
        return _XByteStorageCreate(true, flag, bytes + range.location, range.length, __func__);
    }
    return NULL;
}
XString _Nonnull XStringCreateWithUtf8CString(XObjectFlag flag, const char * _Nonnull cString) {
    XAssert(NULL != cString, __func__, "cString is NULL");
    size_t length = strlen(cString);
    if (length == 0) {
        return XStringEmpty;
    }
    XString str = _XStringCreateWithUtf8Bytes(flag, (const XUInt8 *)cString, length, __func__);
    XAssert(NULL != str, __func__, "cString error");
    return str;
}

XString _Nullable XStringCreateWithBytes(XObjectFlag flag, const XUInt8 * _Nonnull bytes, XSize length, XStringEncoding encode) {
    XAssert(NULL != bytes, __func__, "cString is NULL");
    switch (encode) {
        case XStringEncodingUtf8: {
            if (length == 0) {
                return XStringEmpty;
            }
            return _XStringCreateWithUtf8Bytes(flag, bytes, length, __func__);
        }
            break;
        default:
            return NULL;
            break;
    }
}

XString _Nullable XStringCreateByDecodeData(XObjectFlag flag, XData _Nonnull data, XStringEncoding encode) {
    XAssert(NULL != data, __func__, "data is NULL");
    XByteStorageUnpacked_t v = _XByteStorageUnpack(data, __func__, "not Data instance");
    XAssert(0 == v.isString, __func__, "not Data instance");
    
    switch (encode) {
        case XStringEncodingUtf8: {
            if (0 == v.contentType) {
                XUInt8 * bytes = &(v.content.nano[0]);
                return _XStringCreateWithUtf8Bytes(flag, bytes, v.length, __func__);
            } else if (1 == v.contentType) {
                return _XStringCreateWithUtf8Bytes(flag, v.content.small, v.length, __func__);
            } else if (2 == v.contentType) {
                _XBuffer * buffer = v.content.large;
                XUInt8 * bytes = &(buffer->content[0]);
                XRange range = _XRangeOfString(bytes, v.length);
                if (XIndexNotFound == range.location) {
                    return NULL;
                }
                if (range.length != buffer->size || range.location != 0) {
                    return _XByteStorageCreate(true, flag, bytes + range.location, (XUInt32)range.length, __func__);
                } else {
                    return _XByteStorageCreateWithBuffer(true, flag, buffer, __func__);
                }
            } else {
                XAssert(false, __func__, "unknown error");
            }
        }
            break;
        default:
            return NULL;
            break;
    }
    return NULL;
}

XHashCode XStringHash(XRef _Nonnull ref) {
    return _XByteStorageHash(ref, true, __func__);
}

#pragma mark - Data

#define XDataEmpty ((XData)((uintptr_t)(X_BUILD_TaggedObjectFlag | X_BUILD_TaggedObjectClassData)))

XData _Nonnull XDataCreate(XObjectFlag flag, XUInt8 * _Nullable bytes, XSize length) {
    if (length == 0) {
        return XDataEmpty;
    }
    return _XByteStorageCreate(false, flag, bytes, length, __func__);
}

XData _Nullable XDataCreateByEncodeString(XObjectFlag flag, XString _Nonnull string, XStringEncoding encode) {
    XAssert(NULL != string, __func__, "string is NULL");
    XByteStorageUnpacked_t v = _XByteStorageUnpack(string, __func__, "not String instance");
    XAssert(1 == v.isString, __func__, "not Data instance");
    switch (encode) {
        case XStringEncodingUtf8: {
            if (0 == v.contentType) {
                XUInt8 * bytes = &(v.content.nano[0]);
                return XDataCreate(flag, bytes, v.length);
            } else if (1 == v.contentType) {
                return XDataCreate(flag, v.content.small, v.length);
            } else if (2 == v.contentType) {
                _XBuffer * buffer = v.content.large;
                return _XByteStorageCreateWithBuffer(false, flag, buffer, __func__);
            } else {
                XAssert(false, __func__, "unknown error");
            }
        }
            break;
        default:
            return NULL;
            break;
    }

    return NULL;
}

XHashCode XDataHash(XRef _Nonnull ref) {
    return _XByteStorageHash(ref, false, __func__);
}



