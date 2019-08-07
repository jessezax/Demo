#ifndef __comm_TYPES_H__
#define __comm_TYPES_H__

typedef uint8_t     comm_u8;
typedef uint16_t    comm_u16;
typedef uint32_t    comm_u32;
typedef uint64_t    comm_u64;
typedef __uint128_t comm_u128;

typedef int8_t      comm_s8;
typedef int16_t     comm_s16;
typedef int32_t     comm_s32;
typedef int64_t     comm_s64;
typedef __int128_t  comm_s128;

#if 0
typedef comm_u8 bool;
enum {
    false = 0,
    true  = 1
};
#endif

#define comm_zalloc x_zalloc
#define comm_free   x_free
#define comm_htons  ntohs
#define comm_htonl  ntohl

///短整型数的网络序转主机序
#define comm_ntohs(val)  ((((val) & 0xFF00)>>8) | (((val) & 0x00FF)<<8))


#endif
