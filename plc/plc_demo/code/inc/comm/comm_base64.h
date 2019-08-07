#ifndef __COMM_BASE64_H__
#define __COMM_BASE64_H__

u32 comm_base64_encode(u8 *src_, u32 src_len, char *dst);
u32 comm_base64_decode(char *src, u32 src_len, char *dst);

#endif
