//
//  XHash.c
//  X
//
//  Created by vector on 2020/6/10.
//  Copyright © 2020 haoqi. All rights reserved.
//

#include "XHash.h"


#if defined(_MSC_VER)

#define FORCE_INLINE    __forceinline

#include <stdlib.h>

#define ROTL32(x,y)    _rotl(x,y)
#define ROTL64(x,y)    _rotl64(x,y)

#define BIG_CONSTANT(x) (x)

// Other compilers

#else    // defined(_MSC_VER)

#define FORCE_INLINE static inline


static inline uint32_t rotl32 ( uint32_t x, int8_t r )
{
  return (x << r) | (x >> (32 - r));
}

static inline uint64_t rotl64 ( uint64_t x, int8_t r )
{
  return (x << r) | (x >> (64 - r));
}

#define    ROTL32(x,y)    rotl32(x,y)
#define ROTL64(x,y)    rotl64(x,y)

#define BIG_CONSTANT(x) (x##LLU)

#endif // !defined(_MSC_VER)

//-----------------------------------------------------------------------------
// Finalization mix - force all bits of a hash block to avalanche

FORCE_INLINE uint32_t fmix32 ( uint32_t h )
{
  h ^= h >> 16;
  h *= 0x85ebca6b;
  h ^= h >> 13;
  h *= 0xc2b2ae35;
  h ^= h >> 16;

  return h;
}

//----------

FORCE_INLINE uint64_t fmix64 ( uint64_t k )
{
  k ^= k >> 33;
  k *= BIG_CONSTANT(0xff51afd7ed558ccd);
  k ^= k >> 33;
  k *= BIG_CONSTANT(0xc4ceb9fe1a85ec53);
  k ^= k >> 33;

  return k;
}

//-----------------------------------------------------------------------------

uint32_t __XMurmurHash32(const void * _Nonnull key, uint32_t len, uint32_t seed) {
    uint32_t h1 = seed;

    const uint32_t c1 = 0xcc9e2d51;
    const uint32_t c2 = 0x1b873593;

    //----------
    // body

   const uint32_t * data = (const uint32_t *)key;
   const uint32_t * end = data + (len/4);

    while (data != end) {
        uint32_t k1 = *data++;
        k1 *= c1;
        k1 = ROTL32(k1,15);
        k1 *= c2;

        h1 ^= k1;
        h1 = ROTL32(h1,13);
        h1 = h1 * 5 + 0xe6546b64;
    }

    //----------
    // tail
    
           
    const uint8_t * tail = (const uint8_t *)data;
    uint32_t k1 = 0;

    switch (len & 3) {
    case 3:
        k1 ^= ((uint32_t)tail[2]) << 16;
    case 2:
        k1 ^= ((uint32_t)tail[1]) << 8;
    case 1:
        k1 ^= ((uint32_t)tail[0]);
        k1 *= c1;
        k1 = ROTL32(k1,15);
        k1 *= c2;
        h1 ^= k1;
    };

    //----------
    // finalization

    h1 ^= len;

    h1 = fmix32(h1);
    return h1;
}



// 64-bit hash for 64-bit platforms
uint64_t __XMurmurHash64_rt64(const void * _Nonnull key, uint64_t len, uint32_t seed) {
        const uint64_t m = 0xc6a4a7935bd1e995ULL;
        const int64_t r = 47;
 
        uint64_t h = seed ^ (len * m);
 
        const uint64_t * data = (const uint64_t *)key;
        const uint64_t * end = data + (len/8);
 
        while (data != end)
        {
                uint64_t k = *data++;
 
                k *= m;
                k ^= k >> r;
                k *= m;
 
                h ^= k;
                h *= m;
        }
 
        const uint8_t * data2 = (const uint8_t *)data;
 
        switch (len & 7)
        {
                case 7: h ^= (uint64_t)(data2[6]) << 48;
                case 6: h ^= (uint64_t)(data2[5]) << 40;
                case 5: h ^= (uint64_t)(data2[4]) << 32;
                case 4: h ^= (uint64_t)(data2[3]) << 24;
                case 3: h ^= (uint64_t)(data2[2]) << 16;
                case 2: h ^= (uint64_t)(data2[1]) << 8;
                case 1: h ^= (uint64_t)(data2[0]);
                        h *= m;
        };
 
        h ^= h >> r;
        h *= m;
        h ^= h >> r;
 
        return h;
}
 
// 64-bit hash for 32-bit platforms
uint64_t __XMurmurHash64_rt32(const void * _Nonnull key, uint32_t len, uint32_t seed) {
        const uint32_t m = 0x5bd1e995;
        const int r = 24;
 
        uint32_t h1 = seed ^ len;
        uint32_t h2 = 0;
 
        const uint32_t * data = (const uint32_t *)key;
 
        while (len >= 8)
        {
                uint32_t k1 = *data++;
                k1 *= m; k1 ^= k1 >> r; k1 *= m;
                h1 *= m; h1 ^= k1;
                len -= 4;
 
                uint32_t k2 = *data++;
                k2 *= m; k2 ^= k2 >> r; k2 *= m;
                h2 *= m; h2 ^= k2;
                len -= 4;
        }
 
        if (len >= 4)
        {
                uint32_t k1 = *data++;
                k1 *= m; k1 ^= k1 >> r; k1 *= m;
                h1 *= m; h1 ^= k1;
                len -= 4;
        }
 
        switch(len)
        {
                case 3: h2 ^= ((unsigned char*)data)[2] << 16;
                case 2: h2 ^= ((unsigned char*)data)[1] << 8;
                case 1: h2 ^= ((unsigned char*)data)[0];
                        h2 *= m;
        };
 
        h1 ^= h2 >> 18; h1 *= m;
        h2 ^= h1 >> 22; h2 *= m;
        h1 ^= h2 >> 17; h1 *= m;
        h2 ^= h1 >> 19; h2 *= m;
 
        uint64_t h = h1;
 
        h = (h << 32) | h2;
 
        return h;
}



XUInt32 XMurmurHash32(const XPtr _Nonnull key, XUInt32 len, XUInt32 seed) {
    if (len > 0) {
        XAssert(NULL != key, __func__, "key null");
    }
    return __XMurmurHash32(key, len, seed);
}

// 64-bit hash for 64-bit platforms
XUInt64 XMurmurHash64(const XPtr _Nonnull key, XUInt len, XUInt32 seed) {
    if (len > 0) {
        XAssert(NULL != key, __func__, "key null");
    }

#if CX_TARGET_RT_64_BIT
    return __XMurmurHash64_rt64(key, len, seed);
#else
    return __XMurmurHash64_rt32(key, (uint32_t)len, seed);
#endif
}





//void MurmurHash3_x86_128 ( const void * key, const Py_ssize_t len,
//                           uint32_t seed, void * out )
//{
//  const uint8_t * data = (const uint8_t*)key;
//  const Py_ssize_t nblocks = len / 16;
//
//  uint32_t h1 = seed;
//  uint32_t h2 = seed;
//  uint32_t h3 = seed;
//  uint32_t h4 = seed;
//
//  const uint32_t c1 = 0x239b961b;
//  const uint32_t c2 = 0xab0e9789;
//  const uint32_t c3 = 0x38b34ae5;
//  const uint32_t c4 = 0xa1e38b93;
//
//  //----------
//  // body
//
//  const uint32_t * blocks = (const uint32_t *)(data + nblocks*16);
//
//  for(Py_ssize_t i = -nblocks; i; i++)
//  {
//    uint32_t k1 = getblock(blocks,i*4+0);
//    uint32_t k2 = getblock(blocks,i*4+1);
//    uint32_t k3 = getblock(blocks,i*4+2);
//    uint32_t k4 = getblock(blocks,i*4+3);
//
//    k1 *= c1; k1  = ROTL32(k1,15); k1 *= c2; h1 ^= k1;
//
//    h1 = ROTL32(h1,19); h1 += h2; h1 = h1*5+0x561ccd1b;
//
//    k2 *= c2; k2  = ROTL32(k2,16); k2 *= c3; h2 ^= k2;
//
//    h2 = ROTL32(h2,17); h2 += h3; h2 = h2*5+0x0bcaa747;
//
//    k3 *= c3; k3  = ROTL32(k3,17); k3 *= c4; h3 ^= k3;
//
//    h3 = ROTL32(h3,15); h3 += h4; h3 = h3*5+0x96cd1c35;
//
//    k4 *= c4; k4  = ROTL32(k4,18); k4 *= c1; h4 ^= k4;
//
//    h4 = ROTL32(h4,13); h4 += h1; h4 = h4*5+0x32ac3b17;
//  }
//
//  //----------
//  // tail
//
//  const uint8_t * tail = (const uint8_t*)(data + nblocks*16);
//
//  uint32_t k1 = 0;
//  uint32_t k2 = 0;
//  uint32_t k3 = 0;
//  uint32_t k4 = 0;
//
//  switch(len & 15)
//  {
//  case 15: k4 ^= tail[14] << 16;
//  case 14: k4 ^= tail[13] << 8;
//  case 13: k4 ^= tail[12] << 0;
//           k4 *= c4; k4  = ROTL32(k4,18); k4 *= c1; h4 ^= k4;
//
//  case 12: k3 ^= tail[11] << 24;
//  case 11: k3 ^= tail[10] << 16;
//  case 10: k3 ^= tail[ 9] << 8;
//  case  9: k3 ^= tail[ 8] << 0;
//           k3 *= c3; k3  = ROTL32(k3,17); k3 *= c4; h3 ^= k3;
//
//  case  8: k2 ^= tail[ 7] << 24;
//  case  7: k2 ^= tail[ 6] << 16;
//  case  6: k2 ^= tail[ 5] << 8;
//  case  5: k2 ^= tail[ 4] << 0;
//           k2 *= c2; k2  = ROTL32(k2,16); k2 *= c3; h2 ^= k2;
//
//  case  4: k1 ^= tail[ 3] << 24;
//  case  3: k1 ^= tail[ 2] << 16;
//  case  2: k1 ^= tail[ 1] << 8;
//  case  1: k1 ^= tail[ 0] << 0;
//           k1 *= c1; k1  = ROTL32(k1,15); k1 *= c2; h1 ^= k1;
//  };
//
//  //----------
//  // finalization
//
//  h1 ^= len; h2 ^= len; h3 ^= len; h4 ^= len;
//
//  h1 += h2; h1 += h3; h1 += h4;
//  h2 += h1; h3 += h1; h4 += h1;
//
//  h1 = fmix(h1);
//  h2 = fmix(h2);
//  h3 = fmix(h3);
//  h4 = fmix(h4);
//
//  h1 += h2; h1 += h3; h1 += h4;
//  h2 += h1; h3 += h1; h4 += h1;
//
//  ((uint32_t*)out)[0] = h1;
//  ((uint32_t*)out)[1] = h2;
//  ((uint32_t*)out)[2] = h3;
//  ((uint32_t*)out)[3] = h4;
//}
//
////-----------------------------------------------------------------------------
//
//void MurmurHash3_x64_128 ( const void * key, const Py_ssize_t len,
//                           const uint32_t seed, void * out )
//{
//  const uint8_t * data = (const uint8_t*)key;
//  const Py_ssize_t nblocks = len / 16;
//
//  uint64_t h1 = seed;
//  uint64_t h2 = seed;
//
//  const uint64_t c1 = BIG_CONSTANT(0x87c37b91114253d5);
//  const uint64_t c2 = BIG_CONSTANT(0x4cf5ad432745937f);
//
//  //----------
//  // body
//
//  const uint64_t * blocks = (const uint64_t *)(data);
//
//  for(Py_ssize_t i = 0; i < nblocks; i++)
//  {
//    uint64_t k1 = getblock(blocks,i*2+0);
//    uint64_t k2 = getblock(blocks,i*2+1);
//
//    k1 *= c1; k1  = ROTL64(k1,31); k1 *= c2; h1 ^= k1;
//
//    h1 = ROTL64(h1,27); h1 += h2; h1 = h1*5+0x52dce729;
//
//    k2 *= c2; k2  = ROTL64(k2,33); k2 *= c1; h2 ^= k2;
//
//    h2 = ROTL64(h2,31); h2 += h1; h2 = h2*5+0x38495ab5;
//  }
//
//  //----------
//  // tail
//
//  const uint8_t * tail = (const uint8_t*)(data + nblocks*16);
//
//  uint64_t k1 = 0;
//  uint64_t k2 = 0;
//
//  switch(len & 15)
//  {
//  case 15: k2 ^= uint64_t(tail[14]) << 48;
//  case 14: k2 ^= uint64_t(tail[13]) << 40;
//  case 13: k2 ^= uint64_t(tail[12]) << 32;
//  case 12: k2 ^= uint64_t(tail[11]) << 24;
//  case 11: k2 ^= uint64_t(tail[10]) << 16;
//  case 10: k2 ^= uint64_t(tail[ 9]) << 8;
//  case  9: k2 ^= uint64_t(tail[ 8]) << 0;
//           k2 *= c2; k2  = ROTL64(k2,33); k2 *= c1; h2 ^= k2;
//
//  case  8: k1 ^= uint64_t(tail[ 7]) << 56;
//  case  7: k1 ^= uint64_t(tail[ 6]) << 48;
//  case  6: k1 ^= uint64_t(tail[ 5]) << 40;
//  case  5: k1 ^= uint64_t(tail[ 4]) << 32;
//  case  4: k1 ^= uint64_t(tail[ 3]) << 24;
//  case  3: k1 ^= uint64_t(tail[ 2]) << 16;
//  case  2: k1 ^= uint64_t(tail[ 1]) << 8;
//  case  1: k1 ^= uint64_t(tail[ 0]) << 0;
//           k1 *= c1; k1  = ROTL64(k1,31); k1 *= c2; h1 ^= k1;
//  };
//
//  //----------
//  // finalization
//
//  h1 ^= len; h2 ^= len;
//
//  h1 += h2;
//  h2 += h1;
//
//  h1 = fmix(h1);
//  h2 = fmix(h2);
//
//  h1 += h2;
//  h2 += h1;
//
//  ((uint64_t*)out)[0] = h1;
//  ((uint64_t*)out)[1] = h2;
//}


#define HASHFACTOR 2654435761U

static inline XHashCode __XHashUInt64(XUInt64 i) {
    return (XHashCode)(i);
}

XHashCode XHashUInt64(XUInt64 i) {
    return __XHashUInt64(i);
}
XHashCode XHashSInt64(XSInt64 i) {
    XUInt64 u = 0;
    if (i < 0) {
        u = (XUInt64)(i * -1);
    } else {
        u = (XUInt64)i;
    }
    return __XHashUInt64(u);
}

static inline XHashCode __XHashFloat64(XFloat64 d) {
    //转化成正数
    const XFloat64 positive = (d < 0) ? -d : d;
    
    //四舍五入
    const XFloat64 positiveInt = floor(positive + 0.5);
    
    //小数部分
    const XFloat64 fractional = (positive - positiveInt) * 18446744073709551616.0L;

    XUInt64 result = (XUInt64)fmod(positiveInt, 18446744073709551616.0L);
    if (fractional < 0) {
        result += -((XUInt64)(fabs(fractional)));
    } else if (fractional > 0) {
        result += (XUInt64)fractional;
    }
    return (XHashCode)result;
}

XHashCode XHashFloat64(XFloat64 f) {
    return __XHashFloat64(f);
}
XHashCode XHashFloat32(XFloat32 f) {
    return __XHashFloat64((XFloat64)f);
}

#define ELF_STEP32(B) T1 = (H << 4) + B; T2 = T1 & 0xF0000000; if (T2) T1 ^= (T2 >> 24); T1 &= (~T2); H = T1;
#define ELF_STEP64(B) T1 = (H << 4) + B; T2 = T1 & 0xF000000000000000ULL; if (T2) T1 ^= (T2 >> 56); T1 &= (~T2); H = T1;

#if CX_TARGET_RT_64_BIT
    #define ELF_STEP ELF_STEP64
#else
    #define ELF_STEP ELF_STEP32
#endif


XUInt32 _XELFHashBytes(XUInt8 * _Nullable bytes, XUInt32 length) {
    if (length > 0) {
        assert(bytes);
    }
    XUInt32 H = 0, T1, T2;
    XUInt32 rem = length;
    while (3 < rem) {
    ELF_STEP32(bytes[length - rem]);
    ELF_STEP32(bytes[length - rem + 1]);
    ELF_STEP32(bytes[length - rem + 2]);
    ELF_STEP32(bytes[length - rem + 3]);
    rem -= 4;
    }
    switch (rem) {
    case 3:  ELF_STEP32(bytes[length - 3]);
    case 2:  ELF_STEP32(bytes[length - 2]);
    case 1:  ELF_STEP32(bytes[length - 1]);
    case 0:  ;
    }
    return H;
}

XHashCode XHash(XUInt8 * _Nullable bytes, XUInt length) {
    if (length > 0) {
        assert(bytes);
    } else {
        return 0;
    }
    XUInt H = 0, T1, T2;
    XUInt rem = MIN(256, length);
    
    XUInt8 * lengthBytes = (XUInt8 *)(&length);
    ELF_STEP(lengthBytes[0]);
    ELF_STEP(lengthBytes[1]);
    ELF_STEP(lengthBytes[2]);
    ELF_STEP(lengthBytes[3]);

#if CX_TARGET_RT_64_BIT
    ELF_STEP(lengthBytes[4]);
    ELF_STEP(lengthBytes[5]);
    ELF_STEP(lengthBytes[6]);
    ELF_STEP(lengthBytes[7]);
#endif
    
    while (3 < rem) {
        ELF_STEP(bytes[length - rem]);
        ELF_STEP(bytes[length - rem + 1]);
        ELF_STEP(bytes[length - rem + 2]);
        ELF_STEP(bytes[length - rem + 3]);
        rem -= 4;
    }
    switch (rem) {
        case 3:  ELF_STEP(bytes[length - 3]);
        case 2:  ELF_STEP(bytes[length - 2]);
        case 1:  ELF_STEP(bytes[length - 1]);
        case 0:  ;
    }
    return H;
}

#undef ELF_STEP32
#undef ELF_STEP64
#undef ELF_STEP



unsigned int hash_BPHash(unsigned char *str){
    unsigned int hash = 0;
    while(*str){
        hash = (hash << 7) ^ (*str++);
    }
    return hash;
}

unsigned int hash_FNVHash(unsigned char *str){
    unsigned int hash = 0;
    while(*str){
        hash = (hash * 0x811C9DC5) ^ (*str++);
    }
    return hash;
}

unsigned int hash_BKDRHash(unsigned char *str){
    unsigned int hash = 0;
    while(*str){
        hash = (hash * 131) + (*str++); // 31 131 1313 13131 131313 etc...
    }
    return hash;
}

unsigned int hash_DJBHash(unsigned char *str){
    unsigned int hash = 5381;
    while(*str){
        hash += ((hash << 5) + (*str++));
    }
    return hash;
}

unsigned int hash_JSHash(unsigned char *str){
    unsigned int hash = 1315423911;
    while(*str){
        hash ^= ((hash << 5) + (hash >> 2) + (*str++));
    }
    return hash;
}

unsigned int hash_DEKHash(unsigned char *str){
    unsigned int hash = strlen(str);
    while(*str){
        hash = ((hash << 5) ^ (hash >> 27)) ^ (*str++);
    }
    return hash;
}

unsigned int hash_SDBMHash(unsigned char *str){
    unsigned int hash = 0;
    while(*str){
        hash = (hash << 6) + (hash << 16) + (*str++) - hash;
    }
    return hash;
}

unsigned int hash_RSHash(unsigned char *str){
    unsigned int a = 63689;
    unsigned int hash = 0;
    while(*str){
        hash = hash * a + (*str++);
        a *= 378551;
    }
    return hash;
}

unsigned int hash_APHash(unsigned char *str){
    unsigned int hash = 0;   //0x00000000, 0xAAAAAAAA
    unsigned int flag = 0;
    while(*str){
        if(flag&1){
            hash ^= (~((hash << 11) ^ (*str++) ^ (hash >> 5)));
        }
        else{
            hash ^= ((hash << 7) ^ (*str++) ^ (hash >> 3));
        }
        flag++;
    }
    return hash;
}

unsigned int hash_PJWHash(unsigned char *str){
    unsigned int hash = 0;
    unsigned int test = 0;
    while(*str){
        hash = (hash << 4) + (*str++);
        if((test = hash & 0xF0000000) != 0){
            hash = ((hash ^ (test >> 24)) & 0x0FFFFFFF);
        }
    }
    return hash;
}

unsigned int hash_ELFHash(unsigned char *str){
    unsigned int hash = 0;
    unsigned int test = 0;
    while(*str){
        hash = (hash << 4) + (*str++);
        if((test = hash & 0xF0000000) != 0){
            hash = ((hash ^ (test >> 24)) & (~test));
        }
    }
    return hash;
}



static void siphash_round(uint64_t v[4]) {
  v[0] += v[1];
  v[2] += v[3];
  v[1] = (v[1] << 13) | (v[1] >> (64 - 13));
  v[3] = (v[3] << 16) | (v[3] >> (64 - 16));
  v[1] ^= v[0];
  v[3] ^= v[2];
  v[0] = (v[0] << 32) | (v[0] >> 32);
  v[2] += v[1];
  v[0] += v[3];
  v[1] = (v[1] << 17) | (v[1] >> (64 - 17));
  v[3] = (v[3] << 21) | (v[3] >> (64 - 21));
  v[1] ^= v[2];
  v[3] ^= v[0];
  v[2] = (v[2] << 32) | (v[2] >> 32);
}

uint64_t SIPHASH_24(const uint64_t key[2], const uint8_t *input,
                    size_t input_len) {
  const size_t orig_input_len = input_len;

  uint64_t v[4];
  v[0] = key[0] ^ UINT64_C(0x736f6d6570736575);
  v[1] = key[1] ^ UINT64_C(0x646f72616e646f6d);
  v[2] = key[0] ^ UINT64_C(0x6c7967656e657261);
  v[3] = key[1] ^ UINT64_C(0x7465646279746573);

  while (input_len >= sizeof(uint64_t)) {
    uint64_t m;
    memcpy(&m, input, sizeof(m));
    v[3] ^= m;
    siphash_round(v);
    siphash_round(v);
    v[0] ^= m;

    input += sizeof(uint64_t);
    input_len -= sizeof(uint64_t);
  }

  union {
    uint8_t bytes[8];
    uint64_t word;
  } last_block;
  last_block.word = 0;
  memcpy(last_block.bytes, input, input_len);
  last_block.bytes[7] = orig_input_len & 0xff;

  v[3] ^= last_block.word;
  siphash_round(v);
  siphash_round(v);
  v[0] ^= last_block.word;

  v[2] ^= 0xff;
  siphash_round(v);
  siphash_round(v);
  siphash_round(v);
  siphash_round(v);

  return v[0] ^ v[1] ^ v[2] ^ v[3];
}


void XSipHashStateInit(XSipHashState * _Nonnull state, XUInt64 key[_Nonnull 2]) {
    XAssert(NULL != state, __func__, "");
    XAssert(NULL != key, __func__, "");
    
    uint64_t * v = state->v;
    v[0] = key[0] ^ UINT64_C(0x736f6d6570736575);
    v[1] = key[1] ^ UINT64_C(0x646f72616e646f6d);
    v[2] = key[0] ^ UINT64_C(0x6c7967656e657261);
    v[3] = key[1] ^ UINT64_C(0x7465646279746573);
}

void XSipHashStateCompress(XSipHashState * _Nonnull state, XUInt64 m) {
    XAssert(NULL != state, __func__, "");
    uint64_t * v = state->v;
    
    v[3] ^= m;
    siphash_round(v);
    siphash_round(v);
    v[0] ^= m;
}

void XSipHashStateFinalize(XSipHashState * _Nonnull state, XUInt64 m) {
    XAssert(NULL != state, __func__, "");
    uint64_t * v = state->v;
    
    v[3] ^= m;
    siphash_round(v);
    siphash_round(v);
    v[0] ^= m;
}
