//
//  XRuntimeInternal.h
//  
//
//  Created by vector on 2020/4/4.
//

#ifndef XRuntimeInternal_h
#define XRuntimeInternal_h

#if defined(__cplusplus)
extern "C" {
#endif

#include "XRuntime.h"
#include "XAllocator.h"
#include "XAtomic.h"
#include "XRef.h"
#include "XHash.h"
#include "XLock.h"


typedef XPtr _XDescriptionBuffer;

typedef void (*XObjectDeinit_f)(XObject _Nonnull obj);
typedef void (*XObjectDescribe_f)(XObject _Nonnull obj, _XDescriptionBuffer _Nullable buffer);
typedef XObject _Nonnull (*XObjectCopy_f)(XObject _Nonnull obj);


#if CX_TARGET_RT_64_BIT
    #pragma pack(push, 8)
#else
    #pragma pack(push, 4)
#endif

/*
 TaggedObject32
 refType: 1, value = 1
 taggedContent: {
    class: 2
    objectContent: 28
 }
 flag: 1, value = 1
*/

/*
 TaggedObject64
 refType: 1, value = 1
 taggedContent: 62 {
    class: 2
    objectContent: 60
 }
 flag: 1, value = 1
*/

/*
 flag: 2
 isnumber: 1 total: 3, subtype: 4
 isDate: 1 total: 4
 isDataOrString
 
 
 */


#if CX_TARGET_RT_64_BIT

#define X_BUILD_TaggedMask 0x8000000000000001ULL
#define X_BUILD_TaggedObjectFlag 0x8000000000000001ULL

#define X_BUILD_TaggedObjectClassMask    0x6000000000000000ULL
#define X_BUILD_TaggedObjectClassShift    61ULL

#define X_BUILD_TaggedConstantValueIndexMask 0xFFFFFFFFFFFFFFFULL

#define X_BUILD_TaggedConstantValue   0x0ULL
#define X_BUILD_TaggedObjectClassNumber   0x2000000000000000ULL
#define X_BUILD_TaggedObjectClassDate    0x4000000000000000ULL
#define X_BUILD_TaggedObjectByteStorage   0x6000000000000000ULL

#define X_BUILD_TaggedObjectByteStorageDataFlag   0x1000000000000000ULL

#define X_BUILD_TaggedObjectClassString  0x2000000000000000ULL
#define X_BUILD_TaggedObjectClassData    0x4000000000000000ULL
#define X_BUILD_TaggedObjectContentShift 1ULL

#else

#define X_BUILD_TaggedMask 0x80000001UL
#define X_BUILD_TaggedObjectFlag 0x80000001UL

#define X_BUILD_TaggedObjectClassMask    0x60000000UL
#define X_BUILD_TaggedObjectClassShift   29UL

#define X_BUILD_TaggedConstantValueIndexMask 0xFFFFFFFUL

#define X_BUILD_TaggedConstantValue   0x0UL
#define X_BUILD_TaggedObjectClassNumber  0x20000000UL
#define X_BUILD_TaggedObjectClassDate    0x40000000UL
#define X_BUILD_TaggedObjectByteStorage  0x60000000UL

#define X_BUILD_TaggedObjectByteStorageDataFlag   0x10000000UL

#define X_BUILD_TaggedObjectContentShift 1UL

#endif

#define X_BUILD_TaggedConstantValueMin X_BUILD_TaggedConstantValueNull
#define X_BUILD_TaggedConstantValueMax X_BUILD_TaggedConstantValueBooleanFalse


typedef struct {
    _Atomic(XFastUInt) rcInfo;
} _XObjectCompressedBase;

#define X_BUILD_RcInfoConstantCompressed (X_BUILD_RcMax | X_BUILD_CompressedRcFlag)

#define _XConstantObjectCompressedBaseMake(typeId) {\
    .rcInfo = ATOMIC_VAR_INIT((uintptr_t)(X_BUILD_RcInfoConstantCompressed | (typeId << X_BUILD_CompressedRcTypeShift))),\
}

#define _XConstantObjectBaseMake(type) {\
    .rcInfo = ATOMIC_VAR_INIT(X_BUILD_RcMax),\
    .typeInfo = ATOMIC_VAR_INIT((uintptr_t)(type)),\
}

typedef struct {
    _XObjectCompressedBase _runtime;
    XUInt8 content[sizeof(XUInt)];
} _XCompressedObject;


#pragma mark - XNull
#pragma mark - XBoolean

#pragma mark - XNumber

typedef union {
    XSInt32 s;
    XUInt32 u;
    XFloat32 f;
} _XNumberBits32_u;

typedef union {
    _XNumberBits32_u bits32;
    XNumberBits64_u bits64;
} _XNumberBits_u;


#if CX_TARGET_RT_64_BIT
typedef struct {
    XUInt32 type;
    _XNumberBits32_u bits32;
    XUInt8 extended[0];/* 可能有0、8、16 3种可能 */
} _XNumberContent_t;
#else
typedef struct {
    XUInt32 type;
    _XNumberBits_u bits;
} _XNumberContent_t;
#endif

typedef struct {
    _XObjectCompressedBase _runtime;
    _XNumberContent_t content;
} _XNumber;

#pragma mark - XDate

typedef struct {
    XTimeInterval time;
} _XDateContent_t;

typedef struct {
    _XObjectCompressedBase _runtime;
    _XDateContent_t content;
} _XDate;

#pragma mark - XString
    
typedef struct {
    _Atomic(XFastUInt) _refCount;
    XUInt size;
    XUInt8 content[0];
} _XBuffer;

typedef struct {
    XUInt32 length;
    XUInt32 __xx;
} _XByteStorageContent_t;

typedef struct {
    XUInt32 length;
    XUInt32 offset;
    _Atomic(XFastUInt) hashCode;
    _XBuffer * _Nonnull buffer;
} _XByteStorageContentLarge_t;

typedef struct {
    XUInt32 length;
    XUInt8 extended[4];/* 可能有0、16、48、112、 240 5种可能 */
} _XByteStorageContentSmall_t;

typedef struct {
    XUInt8 content[sizeof(XUInt)];//content[0] 标识长度
} _XByteStorageNano_t;
    
#define _XByteStorageContentBufferSizeMin X_BUILD_UInt(245)

typedef struct {
    XUInt isString: 1;
    XUInt contentType: (XUIntBitsCount - 1);//0 tagged; 1 small; 2 large
    union XByteStorageUnpackedContent_u {
        _XByteStorageContentLarge_t * _Nonnull large;
        _XByteStorageContentSmall_t * _Nonnull small;
        _XByteStorageNano_t nano;
        XUInt __nano;
    } content;
} XByteStorageUnpacked_t;

typedef struct {
    _XObjectCompressedBase _runtime;
    _XByteStorageContent_t content;
} _XByteStorage;

//typedef void (*XCustomRelease_f)(XPtr _Nullable context, XPtr _Nonnull content, XUInt length);
//typedef struct {
//    XPtr _Nullable context;
//    XUInt length;
//    XCustomRelease_f _Nullable customRelease;
//    XPtr _Nonnull content;
//} _XByteCustomContent_t;

typedef _XByteStorage _XString;

#pragma mark - XData

typedef _XByteStorage _XData;



#pragma mark - XValue

/*
32: (4 + 4 + 4 = 12)
4 20 52
64: (8 + 4 + 4 = 16)
16 48
*/

typedef struct {
    XUInt32 clearWhenDealloc: 1;
    XUInt32 contentSize: 31;
    _Atomic(XFastUInt32) hashCode;
    XUInt8 extended[0];
} _XValueContent_t;

typedef struct {
    _XObjectCompressedBase _runtime;
    _XValueContent_t content;
} _XValue;


#pragma mark - XPackage

typedef struct {
    XU8Char * _Nonnull typeName;
    XSize clearWhenDealloc: 1;
    XSize paddingSize: 6;
    XSize contentSize: (XUIntBitsCount - 7);
    XPackageDeinit_f _Nullable deinit;
    XUInt8 extended[0];
} _XPackageContent_t;

typedef struct {
    _XObjectCompressedBase _runtime;
    _XPackageContent_t content;
} _XPackage;

#pragma mark - XWeakPackage

struct __WeakPackage;
    
typedef struct {
    uintptr_t value;//value 不可变
    _Atomic(uintptr_t) table;
    struct __WeakPackage * _Nullable next;
} _WeakPackageContent_t;

typedef struct __WeakPackage {
    _XObjectCompressedBase _runtime;
    _WeakPackageContent_t content;
} _WeakPackage;


#pragma mark - XCollection
typedef struct {
    XPtr _Nonnull collection;
} _XCollectionContent_t;
typedef struct {
    _XObjectCompressedBase _runtime;
    _XCollectionContent_t content;
} _XCollection;

#pragma mark - XArray
typedef _XCollection _XArray;

#pragma mark - XStorage
typedef _XCollection _XStorage;

#pragma mark - XMap
typedef _XCollection _XMap;

#pragma mark - XSet
typedef _XCollection _XSet;

#pragma mark - XObject

typedef struct {
    XObjectBase_s _runtime;
    XUInt8 content[0];
} _XObject;



#pragma pack(pop)

extern void _XWeakPackageRelease(_WeakPackage * _Nonnull WeakPackage);

    
    
    
#pragma mark - runtime

extern XTaggedType _XRefGetTaggedType(XRef _Nonnull ref);
extern XIndex _XHeapRefGetTypeId(XHeapRef _Nonnull ref);
extern const XObjectType_s * _Nonnull _XObjectGetClass(_XObject * _Nonnull object, const char * _Nonnull func);


#pragma mark - runtime-life

extern XSize _XNumberContentDeinit(XPtr _Nonnull content);
extern XSize _XDateContentDeinit(XPtr _Nonnull content);
extern XSize _XByteStorageContentDeinit(XPtr _Nonnull content);

extern XSize _XValueContentDeinit(XPtr _Nonnull content);
extern XSize _XPackageContentDeinit(XPtr _Nonnull content);
extern XSize _XWeakPackageContentDeinit(XPtr _Nonnull content);

extern XSize _XArrayContentDeinit(XPtr _Nonnull content);
extern XSize _XStorageContentDeinit(XPtr _Nonnull content);
extern XSize _XMapContentDeinit(XPtr _Nonnull content);
extern XSize _XSetContentDeinit(XPtr _Nonnull content);


extern void _XRefDeinit(XRef _Nonnull obj);
    
    

#pragma mark - weak

typedef struct {
    XIndex id;
    XUInt lock;
    
} _XWeakTable;
extern void _XWeakTableTryRemove(_XWeakTable * _Nonnull table, XObject _Nonnull value);
extern _XWeakTable * _Nonnull _XWeakTableGet(uintptr_t address);
extern void _XWeakTableLock(_XWeakTable * _Nonnull table);
extern void _XWeakTableUnlock(_XWeakTable * _Nonnull table);

#pragma mark - Hasher

typedef struct __XHasher {
    XIndex cid: 10;
    XIndex elementSize: (XUIntBitsCount - 10);
    XIndex count;
} XHasher_s;

#if defined(__cplusplus)
}  // extern C
#endif

#endif /* XRuntime_h */
