
#ifndef XInteger_h
#define XInteger_h

#include "CXTargetConditionals.h"
#include "XType.h"


static inline uint16_t XUInt16ByteSwap(uint16_t data) {
    return (uint16_t)(data << 8 | data >> 8);
}

static inline uint32_t XUInt32ByteSwap(uint32_t data) {
#if defined(__llvm__)
    data = __builtin_bswap32(data);
#else
    data = (((data ^ (data >> 16 | (data << 16))) & 0xFF00FFFF) >> 8) ^ (data >> 8 | data << 24);
#endif
    return data;
}

static inline uint64_t XUInt64ByteSwap(uint64_t data) {
#if defined(__llvm__)
    return __builtin_bswap64(data);
#else
    union {
        uint64_t ull;
        uint32_t ul[2];
    } u;
    u.ull = data;
    uint32_t t = u.ul[0];
    u.ul[0] = XUInt32ByteSwap(u.ul[1]);
    u.ul[1] = XUInt32ByteSwap(t);
    return u.ull;
#endif
}

#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
static inline uint64_t XUInt64MakeWithBigEndianBytes(uint8_t const block[_Nonnull 8]) {
    uint64_t value = *((uint64_t *)block);
    return XUInt64ByteSwap(value);
}
static inline uint32_t XUInt32MakeWithBigEndianBytes(uint8_t const block[_Nonnull 4]) {
    uint32_t value = *((uint32_t *)block);
    return XUInt32ByteSwap(value);
}
static inline uint64_t XUInt64MakeWithLittleEndianBytes(uint8_t const block[_Nonnull 8]) {
    return *((uint64_t *)block);
}
static inline uint32_t XUInt32MakeWithLittleEndianBytes(uint8_t const block[_Nonnull 4]) {
    return *((uint32_t *)block);
}

static inline void XUInt64ToBigEndianBytes(uint64_t value, uint8_t bytes[_Nonnull 8]) {
    uint64_t v = XUInt64ByteSwap(value);
    *((uint64_t *)bytes) = v;
}
static inline void XUInt32ToBigEndianBytes(uint32_t value, uint8_t bytes[_Nonnull 4]) {
    uint32_t v = XUInt32ByteSwap(value);
    *((uint32_t *)bytes) = v;
}

static inline void XUInt64ToLittleEndianBytes(uint64_t value, uint8_t block[_Nonnull 8]) {
    *((uint64_t *)block) = value;
}
static inline void XUInt32ToLittleEndianBytes(uint32_t value, uint8_t block[_Nonnull 4]) {
    *((uint32_t *)block) = value;
}


#elif __BYTE_ORDER__== __ORDER_BIG_ENDIAN__
static inline uint64_t XUInt64MakeWithBigEndianBytes(uint8_t const block[_Nonnull 8]) {
    return *((uint64_t *)block);
}
static inline uint32_t XUInt32MakeWithBigEndianBytes(uint8_t const block[_Nonnull 4]) {
    return *((uint32_t *)block);
}
static inline uint64_t XUInt64MakeWithLittleEndianBytes(uint8_t const block[_Nonnull 8]) {
    uint64_t value = *((uint64_t *)block);
    return XUInt64ByteSwap(value);
}
static inline uint32_t XUInt32MakeWithLittleEndianBytes(uint8_t const block[_Nonnull 4]) {
    uint32_t value = *((uint32_t *)block);
    return XUInt32ByteSwap(value);
}

static inline void XUInt64ToBigEndianBytes(uint64_t value, uint8_t bytes[_Nonnull 8]) {
    *((uint64_t *)block) = value;
}
static inline void XUInt32ToBigEndianBytes(uint32_t value, uint8_t bytes[_Nonnull 4]) {
    *((uint32_t *)block) = value;
}
static inline void XUInt64ToLittleEndianBytes(uint64_t value, uint8_t block[_Nonnull 8]) {
    uint64_t v = XUInt64ByteSwap(value);
    *((uint64_t *)bytes) = v;
}
static inline void XUInt32ToLittleEndianBytes(uint32_t value, uint8_t block[_Nonnull 4]) {
    uint32_t v = XUInt32ByteSwap(value);
    *((uint32_t *)bytes) = v;
}

#else
#error "DEFINE BIG_ENDIAN OR LITTLE_ENDIAN"
#endif



#if defined(__x86_64__) && !defined(_MSC_VER) //clang _MSVC doesn't support GNU-style inline assembly
// MARK: -- intel 64 asm version

static inline uint64_t XUInt64RotateLeft(uint64_t word, unsigned int offset) {
    __asm__("rolq %%cl,%0"
            :"=r" (word)
            :"0" (word),"c" (offset));
    return word;
}

static inline uint64_t XUInt64RotateRight(uint64_t word, unsigned int offset) {
    __asm__("rorq %%cl,%0"
            :"=r" (word)
            :"0" (word),"c" (offset));
    return word;
}

#else /* Not x86_64  */

// MARK: -- default C version

static inline uint64_t XUInt64RotateLeft(uint64_t word, unsigned int offset) {
    return ( (word<<(offset&63)) | (word>>(64-(offset&63))) );
}

static inline uint64_t XUInt64RotateRight(uint64_t word, unsigned int offset) {
    return ( (word>>(offset&63)) | (word<<(64-(offset&63))) );
}

#endif

// MARK: - 32-bit Rotates

#if defined(_MSC_VER)
// MARK: -- MSVC version

#include <stdlib.h>
#if !defined(__clang__)
#pragma intrinsic(_lrotr,_lrotl)
#endif
#define    XUInt32RotateRight(x,n) _lrotr(x,n)
#define    XUInt32RotateLeft(x,n) _lrotl(x,n)

#elif (defined(__i386__) || defined(__x86_64__))
// MARK: -- intel asm version

static inline uint32_t XUInt32RotateLeft(uint32_t word, unsigned int offset) {
    __asm__ ("roll %%cl,%0"
             :"=r" (word)
             :"0" (word),"c" (offset));
    return word;
}

static inline uint32_t XUInt32RotateRight(uint32_t word, unsigned int offset) {
    __asm__ ("rorl %%cl,%0"
             :"=r" (word)
             :"0" (word),"c" (offset));
    return word;
}

#else

static inline uint32_t XUInt32RotateLeft(uint32_t word, unsigned int offset) {
    return ( (word<<(offset&31)) | (word>>(32-(offset&31))) );
}

static inline uint32_t XUInt32RotateRight(uint32_t word, unsigned int offset) {
    return ( (word>>(offset&31)) | (word<<(32-(offset&31))) );
}

#endif


#endif /* XInteger_h */
