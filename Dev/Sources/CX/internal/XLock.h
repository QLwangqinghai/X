//
//  XLock.h
//  X
//
//  Created by vector on 2020/6/11.
//  Copyright © 2020 haoqi. All rights reserved.
//

#ifndef XLock_h
#define XLock_h

#include "XType.h"


#if CX_TARGET_OS_DARWIN
#import <os/lock.h>

typedef os_unfair_lock XSpinlock_t;

#elif CX_TARGET_OS_LINUX
#include <pthread.h>

typedef pthread_spinlock_t XSpinlock_t;

#else

#error "error"

#endif

extern void XSpinlockInit(XSpinlock_t * _Nonnull lock);
extern void XSpinlockDeinit(XSpinlock_t * _Nonnull lock);
extern void XSpinlockLock(XSpinlock_t * _Nonnull lock);
extern void XSpinlockUnlock(XSpinlock_t * _Nonnull lock);
extern XBool XSpinlockTrylock(XSpinlock_t * _Nonnull lock);


typedef struct {
    XUInt capacity;
    XSpinlock_t table[0];
} XSpinlockTable_s;

extern XSpinlockTable_s * _Nonnull XSpinlockTableCreate(XUInt capacity);
extern void XSpinlockTableDestroy(XSpinlockTable_s * _Nonnull table);
extern XSpinlock_t * _Nonnull XSpinlockTableGet(XSpinlockTable_s * _Nonnull table, XIndex idx);



#endif /* XLock_h */
