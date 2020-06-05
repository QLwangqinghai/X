//
//  main.c
//  XCoreCollection
//
//  Created by vector on 2020/6/5.
//  Copyright © 2020 haoqi. All rights reserved.
//

#include <stdio.h>
#include "XCoreCollection.h"


void printBuffer(uint32_t * _Nonnull buffer, uint32_t count) {
    printf("[");

    for (uint32_t i=0; i<count; i++) {
        if (i == count-1) {
            printf("%u:%u", i, buffer[i]);
        } else {
            printf("%u:%u, ", i, buffer[i]);
        }
    }
    printf("]\n");
}

int main(int argc, const char * argv[]) {
    // insert code here...
    uint32_t bufferSize = 64;
    uint32_t buffer[64] = {};
    uint32_t offset = 23;
    for (uint32_t i=0; i<50; i++) {
        uint32_t idx = (offset + i) % bufferSize;
        buffer[idx] = i + 1;
    }
    printBuffer(buffer, bufferSize);
//    XRange range = {37, 50-37};
//    XRange range1 = {range.location + range.length - 10, 10};
//    XCCCircularBufferMoveForward((uint8_t *)buffer, bufferSize, offset, sizeof(uint32_t), range, 10);
//    printBuffer(buffer, bufferSize);
//    XCCCircularBufferBezero((uint8_t *)buffer, bufferSize, offset, sizeof(uint32_t), range1);
//    printBuffer(buffer, bufferSize);

    
    XRange range = {18, 14};

    XCCCircularBufferMoveBackward((uint8_t *)buffer, bufferSize, offset, sizeof(uint32_t), range, 10);
    printBuffer(buffer, bufferSize);
    
    XRange range1 = {range.location, 10};
    XCCCircularBufferBezero((uint8_t *)buffer, bufferSize, offset, sizeof(uint32_t), range1);
    printBuffer(buffer, bufferSize);
    
    
    return 0;
}

