//
//  XClass.c
//  X
//
//  Created by clf on 2020/5/19.
//  Copyright © 2020 haoqi. All rights reserved.
//

#include "XClass.h"
#include "XRef.h"
#include "internal/XRefInternal.h"
#include "XObject.h"


void _XRefConstantValueDeinit(XRef _Nonnull obj) {
    XAssert(false, __func__, "");
}

XIndex _XTypeIdOfClass(const XType_s * _Nonnull cls) {
    uintptr_t base = (uintptr_t)(&(_XClassTable[0]));
    uintptr_t end = base + sizeof(XType_s);
    uintptr_t c = (uintptr_t)cls;
    if (c < base || c >= end) {
        return XUIntMax;
    } else {
        uintptr_t offset = c - base;
        if (offset % sizeof(XType_s) == 0) {
            XIndex id = offset / sizeof(XType_s);
            return id;
        } else {
            return XIndexNotFound;
        }
    }
}



XHashCode _XClassHash(XRef _Nonnull obj) {
    return XAddressHash(obj);
}
XBool _XClassEqual(XRef _Nonnull lhs, XRef _Nonnull rhs) {
    return lhs == rhs;
}
void _XClassDescribe(XRef _Nonnull lhs, XRef _Nonnull rhs) {

}

XHashCode _XValueHash(XRef _Nonnull obj) {

    
    return 0;
}
XBool _XValueEqual(XRef _Nonnull lhs, XRef _Nonnull rhs) {

    
    return 0;
}
void _XValueDeinit(XRef _Nonnull obj) {


}
void _XValueDescribe(XRef _Nonnull obj, _XDescriptionBuffer _Nonnull buffer) {


}

XHashCode _XCollectionHash(XRef _Nonnull obj) {
    XCompressedType typeId = XHeapRefGetCompressedType(obj);
    XAssert((typeId >= XCompressedTypePackage && typeId <= XCompressedTypeMax), __func__, "");

    return 0;
}
XBool _XCollectionEqual(XRef _Nonnull lhs, XRef _Nonnull rhs) {
    XCompressedType typeId0 = XHeapRefGetCompressedType(lhs);
    XAssert((typeId0 >= XCompressedTypePackage && typeId0 <= XCompressedTypeMax), __func__, "");
    XCompressedType typeId1 = XHeapRefGetCompressedType(rhs);
    XAssert((typeId1 >= XCompressedTypePackage && typeId1 <= XCompressedTypeMax), __func__, "");
    return (lhs == rhs);
}
void _XCollectionDeinit(XRef _Nonnull obj) {
    XCompressedType typeId = XHeapRefGetCompressedType(obj);
    XAssert((typeId >= XCompressedTypePackage && typeId <= XCompressedTypeMax), __func__, "");

}
void _XCollectionDescribe(XRef _Nonnull obj, _XDescriptionBuffer _Nonnull buffer) {
    XCompressedType typeId = XHeapRefGetCompressedType(obj);
    XAssert((typeId >= XCompressedTypePackage && typeId <= XCompressedTypeMax), __func__, "");

}

XHashCode _XObjectHash(XRef _Nonnull obj) {
    /*const XObjectType_s * type = */_XObjectGetClass(obj, __func__);
    return XAddressHash(obj);
}
XBool _XObjectEqual(XRef _Nonnull lhs, XRef _Nonnull rhs) {
    /*const XObjectType_s * type = */_XObjectGetClass(lhs, __func__);
    /*const XObjectType_s * type = */_XObjectGetClass(rhs, __func__);
    return lhs == rhs;
}
void _XObjectDeinit(XRef _Nonnull obj) {
    XAssert(NULL != obj, __func__, "");
    const XObjectType_s * type = _XObjectGetClass(obj, __func__);

}
void _XObjectDescribe(XRef _Nonnull obj, _XDescriptionBuffer _Nonnull buffer) {
    XAssert(NULL != obj, __func__, "");
    const XObjectType_s * type = _XObjectGetClass(obj, __func__);

}


#define _XRefKindMake(Type) \
{\
.hash = _X##Type##Hash,\
.equal = _X##Type##Equal,\
.deinit = _X##Type##Deinit,\
.describe = _X##Type##Describe,\
}


#define X_BUILD_RefKindId_Class X_BUILD_UInt(0)
#define X_BUILD_RefKindId_ConstantValue X_BUILD_UInt(1)
#define X_BUILD_RefKindId_Value X_BUILD_UInt(2)
#define X_BUILD_RefKindId_Collection X_BUILD_UInt(3)
#define X_BUILD_RefKindId_Object X_BUILD_UInt(4)


const XCompressedType XCompressedTypeNumber = X_BUILD_CompressedType_Number;
const XCompressedType XCompressedTypeDate = X_BUILD_CompressedType_Date;
const XCompressedType XCompressedTypeString = X_BUILD_CompressedType_String;
const XCompressedType XCompressedTypeData = X_BUILD_CompressedType_Data;
const XCompressedType XCompressedTypeValue = X_BUILD_CompressedType_Value;
const XCompressedType XCompressedTypePackage = X_BUILD_CompressedType_Package;
const XCompressedType XCompressedTypeWeakPackage = X_BUILD_CompressedType_WeakPackage;
const XCompressedType XCompressedTypeArray = X_BUILD_CompressedType_Array;
const XCompressedType XCompressedTypeStorage = X_BUILD_CompressedType_Storage;
const XCompressedType XCompressedTypeMap = X_BUILD_CompressedType_Map;
const XCompressedType XCompressedTypeSet = X_BUILD_CompressedType_Set;
const XCompressedType XCompressedTypeMax = XCompressedTypeSet;

#define _XClassMake(Name) \
{\
._runtime = {\
.rcInfo = ATOMIC_VAR_INIT(X_BUILD_RcMax),\
.typeInfo = (uintptr_t)_XClassTable,\
},\
.base = {\
.name = #Name,\
.kind = XRefKindOfNormal(Name),\
},\
}

#define _XClassMakeConstantValue(Name) _XClassMake(Name)
#define _XClassMakeValue(Name) _XClassMake(Name)
#define _XClassMakeCollection(Name) _XClassMake(Name)
#define _XClassMakeObject(Name) _XClassMake(Name)

const XType_s _XClassTable[] __attribute__((aligned(64))) = {
    //ConstantValue
    _XClassMake(MetaType),
    _XClassMakeConstantValue(Null),
    _XClassMakeConstantValue(Boolean),
    
    //Value
    _XClassMakeValue(Number),
    _XClassMakeValue(Date),
    _XClassMakeValue(String),
    _XClassMakeValue(Data),
    _XClassMakeValue(Value),
    
    //Collection
    _XClassMakeCollection(Package),
    _XClassMakeCollection(WeakPackage),
    _XClassMakeCollection(Array),
    _XClassMakeCollection(Storage),
    _XClassMakeCollection(Map),
    _XClassMakeCollection(Set),

};

const XObjectType_s XObjectTypeRootObject __attribute__((aligned(64))) = {
    ._runtime = {
    .rcInfo = ATOMIC_VAR_INIT(X_BUILD_RcMax),
    .typeInfo = (uintptr_t)_XClassTable,
    },
    .base = {
    .name = "Object",
    .kind = X_BUILD_TypeKindObject,
    },
};


XCompressedType XCompressedTypeOfClass(XClass _Nonnull cls) {
    uintptr_t base = (uintptr_t)(&(_XClassTable[0]));
    uintptr_t end = base + sizeof(XType_s);
    uintptr_t c = (uintptr_t)cls;
    if (c < base || c >= end) {
        return XUIntMax;
    } else {
        uintptr_t offset = c - base;
        if (offset % sizeof(XType_s) == 0) {
            XIndex id = offset / sizeof(XType_s);
            if (id < X_BUILD_TypeId_CompressedTypeMin + X_BUILD_TypeId_CompressedTypeCount && id >= X_BUILD_TypeId_CompressedTypeMin) {
                return id - X_BUILD_TypeId_CompressedTypeMin;
            } else {
                return XUIntMax;
            }
        } else {
            return XUIntMax;
        }
    }
}



const XClass _Nonnull XClassType = XClassOf(Class);
const XClass _Nonnull XClassNull = XClassOf(Null);
const XClass _Nonnull XClassBoolean = XClassOf(Boolean);
const XClass _Nonnull XClassNumber = XClassOf(Number);
const XClass _Nonnull XClassDate = XClassOf(Date);
const XClass _Nonnull XClassString = XClassOf(String);
const XClass _Nonnull XClassData = XClassOf(Data);
const XClass _Nonnull XClassValue = XClassOf(Value);
const XClass _Nonnull XClassPackage = XClassOf(Package);
const XClass _Nonnull XClassWeakPackage = XClassOf(WeakPackage);
const XClass _Nonnull XClassArray = XClassOf(Array);
const XClass _Nonnull XClassStorage = XClassOf(Storage);
const XClass _Nonnull XClassMap = XClassOf(Map);
const XClass _Nonnull XClassSet = XClassOf(Set);

const XClass _Nonnull XClassObject = (const XClass)&XObjectTypeRootObject;

