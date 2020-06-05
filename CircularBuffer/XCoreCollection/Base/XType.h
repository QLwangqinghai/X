//
//  XType.h
//  XCoreCollection
//
//  Created by vector on 2020/6/5.
//  Copyright © 2020 haoqi. All rights reserved.
//

#ifndef XType_h
#define XType_h

#include <stdio.h>
#include <string.h>

#define false 0
#define true 1
typedef void * XPtr;

typedef uint32_t XIndex;

typedef uint32_t XBool;
typedef uint8_t XUInt8;

typedef struct {
    XIndex location;
    XIndex length;
} XRange;

#define XAssert(cond, func, desc) {\
    if (!(cond)) {\
        fprintf(stderr, "%s error, %s", func, desc);\
        abort();\
    }\
}

#endif /* XType_h */
