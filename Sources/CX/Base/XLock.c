//
//  XLock.c
//  X
//
//  Created by vector on 2020/6/11.
//  Copyright © 2020 haoqi. All rights reserved.
//

#include "../include/Base/XLockInternal.h"
#include "../include/Base/XMemory.h"



#if CX_TARGET_OS_DARWIN

void XSpinlockInit(XSpinlock_t * _Nonnull lock) {
    XAssert(NULL != lock, "");
    *lock = OS_UNFAIR_LOCK_INIT;
}
void XSpinlockDeinit(XSpinlock_t * _Nonnull lock) {
    XAssert(NULL != lock, "");
}
void XSpinlockLock(XSpinlock_t * _Nonnull lock) {
    XAssert(NULL != lock, "");
    os_unfair_lock_lock(lock);
}
void XSpinlockUnlock(XSpinlock_t * _Nonnull lock) {
    XAssert(NULL != lock, "");
    os_unfair_lock_unlock(lock);
}
XBool XSpinlockTrylock(XSpinlock_t * _Nonnull lock) {
    XAssert(NULL != lock, "");
    return os_unfair_lock_trylock(lock);
}

#endif

#if CX_TARGET_OS_LINUX

void XSpinlockInit(XSpinlock_t * _Nonnull lock) {
    XAssert(NULL != lock, "");
    XAssert(0 == pthread_spin_init(lock, PTHREAD_PROCESS_PRIVATE), __func__, "");
}
void XSpinlockDeinit(XSpinlock_t * _Nonnull lock) {
    XAssert(NULL != lock, "");
    XAssert(0 == pthread_spin_destroy(lock), __func__, "");
}
void XSpinlockLock(XSpinlock_t * _Nonnull lock) {
    XAssert(NULL != lock, "");
    XAssert(0 == pthread_spin_lock(lock), __func__, "");
}
void XSpinlockUnlock(XSpinlock_t * _Nonnull lock) {
    XAssert(NULL != lock, "");
    XAssert(0 == pthread_spin_unlock(lock), __func__, "");
}
XBool XSpinlockTrylock(XSpinlock_t * _Nonnull lock) {
    XAssert(NULL != lock, "");
    return 0 == pthread_spin_unlock(lock);
}

#endif





XSpinlockTable_s * _Nonnull XSpinlockTableCreate(XUInt capacity) {
    XSpinlockTable_s * table = XAllocate(sizeof(XSpinlockTable_s) + sizeof(XSpinlock_t) * capacity);
    table->capacity = capacity;
    for (XIndex i=0; i<capacity; i++) {
        XSpinlockInit(&(table->table[i]));
    }
    return table;
}
void XSpinlockTableDestroy(XSpinlockTable_s * _Nonnull table) {
    XDeallocate(table);
}
XSpinlock_t * _Nonnull XSpinlockTableGet(XSpinlockTable_s * _Nonnull table, XIndex idx) {
    XAssert(NULL != table, "");
    XAssert(idx < table->capacity, "");
    return &(table->table[idx]);
}
