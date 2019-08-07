#ifndef __X_TYPES_H__
#define __X_TYPES_H__

//#define __packed__ __attribute__((packed))


#if __GNUC__ == 2 && __GNUC_MINOR__ < 96
#define __builtin_expect(x, expected_value) (x)
#endif

#define likely(x)   __builtin_expect((x),1)
#define unlikely(x) __builtin_expect((x),0)

typedef __int128_t  s128;
typedef int64_t     s64;
typedef int32_t     s32;
typedef int16_t     s16;
typedef int8_t      s8;


typedef __uint128_t u128;
typedef uint64_t    u64;
typedef uint32_t    u32;
typedef uint16_t    u16;
typedef uint8_t     u8;


typedef __uint128_t be128;
typedef uint64_t    be64;
typedef uint32_t    be32;
typedef uint16_t    be16;


#endif
