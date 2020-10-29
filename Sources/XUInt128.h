
#ifndef XUInt128_h
#define XUInt128_h

#include "XInteger.h"


typedef struct _CUInt128 {
    uint8_t content[16];
} CUInt128_t;

extern const CUInt128_t CUInt128Zero;

#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
#define CUInt128High64Offset 8
#define CUInt128Low64Offset 0
#elif __BYTE_ORDER__== __ORDER_BIG_ENDIAN__
#define CUInt128High64Offset 0
#define CUInt128Low64Offset 8
#else
#error "DEFINE BIG_ENDIAN OR LITTLE_ENDIAN"
#endif

#define _kCUInt128GetLow64Ptr(a) ((uint64_t *)(&((a).content[CUInt128Low64Offset])))
#define _kCUInt128GetHigh64Ptr(a) ((uint64_t *)(&((a).content[CUInt128High64Offset])))

static inline uint64_t * _Nonnull _CUInt128GetLow64Ptr(CUInt128_t * _Nonnull a) {
    return ((uint64_t *)(&(a->content[CUInt128Low64Offset])));
}
static inline uint64_t * _Nonnull _CUInt128GetHigh64Ptr(CUInt128_t * _Nonnull a) {
    return ((uint64_t *)(&(a->content[CUInt128High64Offset])));
}

//~a
static inline CUInt128_t CUInt128Not(CUInt128_t a) {
    uint64_t * l = _kCUInt128GetLow64Ptr(a);
    uint64_t * h = _kCUInt128GetHigh64Ptr(a);
    *l = ~(*l);
    *h = ~(*h);
    return a;
}

//a | b
static inline CUInt128_t CUInt128Or(CUInt128_t a, CUInt128_t b) {
    uint64_t * l = _kCUInt128GetLow64Ptr(a);
    uint64_t * h = _kCUInt128GetHigh64Ptr(a);
    *l = (*l) | (*_kCUInt128GetLow64Ptr(b));
    *h = (*h) | (*_kCUInt128GetHigh64Ptr(b));
    return a;
}

//a & b
static inline CUInt128_t CUInt128And(CUInt128_t a, CUInt128_t b) {
    uint64_t * l = _kCUInt128GetLow64Ptr(a);
    uint64_t * h = _kCUInt128GetHigh64Ptr(a);
    *l = (*l) & (*_kCUInt128GetLow64Ptr(b));
    *h = (*h) & (*_kCUInt128GetHigh64Ptr(b));
    return a;
}

//a ^ b
static inline CUInt128_t CUInt128Xor(CUInt128_t a, CUInt128_t b) {
    uint64_t * l = _kCUInt128GetLow64Ptr(a);
    uint64_t * h = _kCUInt128GetHigh64Ptr(a);
    *l = (*l) ^ (*_kCUInt128GetLow64Ptr(b));
    *h = (*h) ^ (*_kCUInt128GetHigh64Ptr(b));
    return a;
}

static inline CUInt128_t CUInt128MakeWithUInt64(uint64_t high, uint64_t low) {
    CUInt128_t v = CUInt128Zero;
    *(_kCUInt128GetLow64Ptr(v)) = low;
    *(_kCUInt128GetHigh64Ptr(v)) = high;
    return v;
}

static inline CUInt128_t CUInt128Shl(CUInt128_t word, unsigned int offset) {
    uint64_t * l = _kCUInt128GetLow64Ptr(word);
    uint64_t * h = _kCUInt128GetHigh64Ptr(word);
    if (offset < 64) {
        uint64_t t = (*l) >> (64 - offset);
        *h = ((*h) << offset) | t;
        *l = (*l) << offset;
    } else if (offset < 128) {
        *h = (*l) << (offset - 64);
        *l = 0;
    }
    return word;
}

static inline CUInt128_t CUInt128Shr(CUInt128_t word, unsigned int offset) {
    uint64_t * l = _kCUInt128GetLow64Ptr(word);
    uint64_t * h = _kCUInt128GetHigh64Ptr(word);
    if (offset < 64) {
        uint64_t t = (*h) << (64 - offset);
        *h = (*h) >> offset;
        *l = ((*l) >> offset) | t;
    } else if (offset < 128) {
        *h = 0;
        *l = (*h) >> (offset - 64);
    }
    return word;
}

static inline CUInt128_t CUInt128RotateLeft(CUInt128_t word, unsigned int offset) {
    return CUInt128Or(CUInt128Shl(word, offset&127), CUInt128Shr(word, 128-(offset&127)));
}

static inline CUInt128_t CUInt128RotateRight(CUInt128_t word, unsigned int offset) {
    return CUInt128Or(CUInt128Shr(word, offset&127), CUInt128Shl(word, 128-(offset&127)));
}

static inline CUInt128_t CUInt128AddUInt64(CUInt128_t a, uint64_t b) {
    uint64_t * h = _kCUInt128GetHigh64Ptr(a);
    uint64_t * l = _kCUInt128GetLow64Ptr(a);
    uint64_t carry = 0;
    *l = *l + b;
    if (*l < b) {
        carry = 1;
        *h = *h + carry;
    }
    return a;
}


static inline CUInt128_t CUInt128Add(CUInt128_t a, CUInt128_t b) {
    uint64_t * h = _kCUInt128GetHigh64Ptr(a);
    uint64_t * l = _kCUInt128GetLow64Ptr(a);
    uint64_t bh = *_kCUInt128GetHigh64Ptr(b);
    uint64_t bl = *_kCUInt128GetLow64Ptr(b);
    uint64_t carry = 0;
    *l = *l + bl;
    if (*l < bl) {
        carry = 1;
    }
    *h = *h + bh + carry;
    return a;
}

//a-b
static inline CUInt128_t CUInt128Sub(CUInt128_t a, CUInt128_t b) {
    uint64_t * h = _kCUInt128GetHigh64Ptr(a);
    uint64_t * l = _kCUInt128GetLow64Ptr(a);
    uint64_t bh = *_kCUInt128GetHigh64Ptr(b);
    uint64_t bl = *_kCUInt128GetLow64Ptr(b);

    uint64_t carry = 0;
    if (*l < bl) {
        carry = 1;
    }
    *l = *l - bl;
    *h = *h - bh - carry;
    return a;
}


static inline CUInt128_t CUInt128Opposite(CUInt128_t word) {
    return CUInt128Sub(CUInt128Zero, word);
}

static inline CUInt128_t CUInt128ByteSwap(CUInt128_t data) {
    CUInt128_t r = {};
    *((uint64_t *)(&(r.content[0]))) = XUInt64ByteSwap(*((uint64_t *)(&(data.content[8]))));
    *((uint64_t *)(&(r.content[8]))) = XUInt64ByteSwap(*((uint64_t *)(&(data.content[0]))));
    return r;
}


#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
static inline CUInt128_t CUInt128MakeWithBigEndianBytes(uint8_t const block[_Nonnull 16]) {
    CUInt128_t value = {};
    memcpy(&(value.content[0]), block, 16);
    return CUInt128ByteSwap(value);
}

static inline void CUInt128ToBigEndianBytes(CUInt128_t value, uint8_t bytes[_Nonnull 16]) {
    CUInt128_t v = CUInt128ByteSwap(value);
    memcpy(bytes, &(v.content[0]), 16);
}
static inline void CUInt128ToLittleEndianBytes(CUInt128_t value, uint8_t bytes[_Nonnull 16]) {
    memcpy(bytes, &(value.content[0]), 16);
}


#elif __BYTE_ORDER__== __ORDER_BIG_ENDIAN__
static inline CUInt128_t CUInt128MakeWithBigEndianBytes(uint8_t const block[_Nonnull 16]) {
    CUInt128_t value = {};
    memcpy(&(value.content[0]), block, 16);
    return value;
}

static inline void CUInt128ToBigEndianBytes(CUInt128_t value, uint8_t bytes[_Nonnull 16]) {
    memcpy(bytes, &(value.content[0]), 16);
}
static inline void CUInt128ToLittleEndianBytes(CUInt128_t value, uint8_t bytes[_Nonnull 16]) {
    CUInt128_t v = CUInt128ByteSwap(value);
    memcpy(bytes, &(v.content[0]), 16);
}

#else
#error "DEFINE BIG_ENDIAN OR LITTLE_ENDIAN"
#endif



#endif /* XUInt128_h */
