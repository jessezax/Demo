#include "libx.h"
#include "comm.h"

//static char base64_map[65] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789!-";
static char base64_map[65] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

static char base64_decode_map[256] = {
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255,  62, 255, 255, 255,  63,  52,  53,  54,  55,  56,  57,  58,  59,  60,  61, 255, 255,
    255,   0, 255, 255, 255,  0,   1,    2,   3,   4,   5,   6,   7,   8,   9,  10,  11,  12,  13,  14,
    15,   16,  17,  18,  19,  20,  21,  22,  23,  24,  25, 255, 255, 255, 255, 255, 255,  26,  27,  28,
    29,   30,  31,  32,  33,  34,  35,  36,  37,  38,  39,  40,  41,  42,  43,  44,  45,  46,  47,  48,
    49,   50,  51, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255
};

u32 comm_base64_encode(u8 *src_, u32 src_len, char *dst)
{
    u32 i = 0, j = 0;
    u8 *src = src_;

    for (;i<src_len - src_len % 3; i += 3)
    {
        dst[j++] = base64_map[(int)((src[i] >> 2) & 0x3f)];
        dst[j++] = base64_map[(int)(((src[i] << 4) | (src[i + 1] >> 4)) & 0x3f)];
        dst[j++] = base64_map[(int)(((src[i + 1] << 2) | (src[i + 2] >> 6 )) & 0x3f)];
        dst[j++] = base64_map[(int)(src[i + 2] & 0x3f)];
    }

    if (src_len % 3 == 1)
    {
        dst[j++] = base64_map[(int)((src[i] >> 2) & 0x3f)];
        dst[j++] = base64_map[(int)((src[i] << 4) & 0x3f)];
        dst[j++] = '=';
        dst[j++] = '=';
    }
    else if (src_len % 3 == 2)
    {
        dst[j++] = base64_map[(int)((src[i] >> 2) & 0x3f)];
        dst[j++] = base64_map[(int)(((src[i] << 4) | (src[i + 1] >> 4)) & 0x3f)];
        dst[j++] = base64_map[(int )((src[i + 1] << 2) & 0x3f)];
        dst[j++] = '=';
    }

    dst[j] = '\0';
    return (j);
}

u32 comm_base64_decode(char *src, u32 src_len, char *dst)
{
    u32 i = 0, j = 0;

    for (; i < src_len; i += 4)
    {
        dst[j++] = base64_decode_map[(int)src[i]] << 2 | base64_decode_map[(int)src[i + 1]] >> 4;
        dst[j++] = base64_decode_map[(int)src[i + 1]] << 4 | base64_decode_map[(int)src[i + 2]] >> 2;
        dst[j++] = base64_decode_map[(int)src[i + 2]] << 6 | base64_decode_map[(int)src[i + 3]];
    }

    dst[j] = '\0';
    return (j);
}

