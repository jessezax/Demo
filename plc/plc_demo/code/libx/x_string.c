#include "libx.h"

unsigned char __string_dup(const char *src,unsigned int len,t_string * dst)
{
    char * p;
    if (len == 0) {
        set_null_string(dst);
        return true;
    }

    p = x_malloc(len + 1);
    if (!p) {
        return false;
    }
    if (len) {
        memcpy(p,src,len);
    }
    p[len] = 0;
    set_string(dst,p,len);
    dst->dup = true;
    return true;
}

unsigned char string_dup(t_string * src,t_string * dst)
{
    return __string_dup(src->data,src->len,dst);
}

void string_free(t_string * p)
{
    if (p->dup && p->data) {
        x_free(p->data);
        set_null_string(p);
    }
}

char * lnindex(char * s,u32 n, char c)
{
    while (n--)
    {
        if (*s == c)
        {
            return (char *)s;
        }
        s++;
    }
    return NULL;
}

bool atob(const char *line, int n,bool * ret)
{
    if (n == const_strlen("false") && !strncasecmp(line,"false",n)) {
        *ret = false;
        return true;
    } else if (n == const_strlen("true") && !strncasecmp(line,"true",n)) {
        *ret = true;
        return true;
    }
    return false;
}

//success return ture,failure return false;
bool atou8(const char *line, int n,u8 * ret)
{
    u64  value;

    if (n == 0)
    {
        return false;
    }

    for (value = 0; n--; line++)
    {
        if (*line < '0' || *line > '9')
        {
            return false;
        }

        value = value * 10 + (*line - '0');
        if (value > 0xff) {
            return false;
        }
    }
    *ret = value;
    return true;
}

//success return ture,failure return false;
bool atou16(const char *line, int n,u16 * ret)
{
    u64  value;

    if (n == 0)
    {
        return false;
    }

    for (value = 0; n--; line++)
    {
        if (*line < '0' || *line > '9')
        {
            return false;
        }

        value = value * 10 + (*line - '0');
        if (value > 0xffff) {
            return false;
        }
    }
    *ret = value;
    return true;
}

//success return ture,failure return false;
bool atou32(const char *line, int n,u32 * ret)
{
    u64  value;

    if (n == 0)
    {
        return false;
    }

    for (value = 0; n--; line++)
    {
        if (*line < '0' || *line > '9')
        {
            return false;
        }

        value = value * 10 + (*line - '0');
        if (value > 0xffffffff) {
            return false;
        }
    }
    *ret = value;
    return true;
}

//success return ture,failure return false;
bool atou64(const char *line, int n,u64 * ret)
{
    u64  value;

    if (n == 0)
    {
        return false;
    }

    for (value = 0; n--; line++)
    {
        if (*line < '0' || *line > '9')
        {
            return false;
        }

        value = value * 10 + (*line - '0');
    }
    *ret = value;
    return true;
}

//success return ture,failure return false;
bool atos8(const char *line, int n,s8 * ret)
{
    u64  value;
    if (n == 0)
    {
        return false;
    }

    char sign = 0;
    if (!sign) {
        if (*line == '-') {
            sign = -1;
            n--;
            line++;
        } else {
            sign = 1;
        }
    }

    for (value = 0; n--; line++)
    {
        if (*line < '0' || *line > '9')
        {
            return false;
        }

        value = value * 10 + (*line - '0');
        if (value > 0x7f) {
            return false;
        }
    }
    *ret = value*sign;
    return true;
}

//success return ture,failure return false;
bool atos16(const char *line, int n,s16 * ret)
{
    u64  value;

    if (n == 0)
    {
        return false;
    }

    char sign = 0;
    if (!sign) {
        if (*line == '-') {
            sign = -1;
            n--;
            line++;
        } else {
            sign = 1;
        }
    }

    for (value = 0; n--; line++)
    {
        if (*line < '0' || *line > '9')
        {
            return false;
        }

        value = value * 10 + (*line - '0');
        if (value > 0x7fff) {
            return false;
        }
    }
    *ret = value*sign;
    return true;
}

//success return ture,failure return false;
bool atos32(const char *line, int n,s32 * ret)
{
    u64  value;

    if (n == 0)
    {
        return false;
    }

    char sign = 0;
    if (!sign) {
        if (*line == '-') {
            sign = -1;
            n--;
            line++;
        } else {
            sign = 1;
        }
    }

    for (value = 0; n--; line++)
    {
        if (*line < '0' || *line > '9')
        {
            return false;
        }

        value = value * 10 + (*line - '0');
        if (value > 0x7fffffff) {
            return false;
        }
    }
    *ret = value*sign;
    return true;
}

//success return ture,failure return false;
bool atos64(const char *line, int n,s64 * ret)
{
    u64  value;

    if (n == 0)
    {
        return false;
    }

    char sign = 0;
    if (!sign) {
        if (*line == '-') {
            sign = -1;
            n--;
            line++;
        } else {
            sign = 1;
        }
    }

    for (value = 0; n--; line++)
    {
        if (*line < '0' || *line > '9')
        {
            return false;
        }

        value = value * 10 + (*line - '0');
    }
    *ret = value*sign;
    return true;
}



//success return ture,failure return false;
bool atosz(const char *line, int n,ssize_t * ret)
{
    ssize_t  value;

    if (n == 0)
    {
        return false;
    }

    for (value = 0; n--; line++)
    {
        if (*line < '0' || *line > '9')
        {
            return false;
        }

        value = value * 10 + (*line - '0');
    }
    *ret = value;
    return true;
}

//success return ture,failure return false;
bool atoof(const char *line, int n,off_t * ret)
{
    off_t  value;

    if (n == 0)
    {
        return false;
    }

    for (value = 0; n--; line++)
    {
        if (*line < '0' || *line > '9')
        {
            return false;
        }

        value = value * 10 + (*line - '0');
    }
    *ret = value;
    return true;
}

//success return ture,failure return false;
bool atotm(const char *line, int n,time_t * ret)
{
    time_t  value;

    if (n == 0)
    {
        return false;
    }

    for (value = 0; n--; line++)
    {
        if (*line < '0' || *line > '9')
        {
            return false;
        }

        value = value * 10 + (*line - '0');
    }
    *ret = value;
    return true;
}

bool __hex2u8(char hex,u8 * ret)
{
    if (hex >= '0' && hex <= '9') {
        *ret = hex - '0';
        return true;
    }
    if (hex >= 'a' && hex <= 'f') {
        *ret = hex - 'a' + 10;
        return true;
    }
    if (hex >= 'A' && hex <= 'F') {
        *ret = hex - 'A' + 10;
        return true;
    }
    return false;
}

bool hex2u8(char * hex,int n,u8 * ret)
{
    u64 val;
    u8  code;
    if (hex[0] == '0' && (hex[1] == 'x' || hex[1] == 'X')) {
        hex += 2;
        n -= 2;
    }
    if (n > 2) {
        return false;
    }
    for (val = 0;n--;hex++) {
        if (!__hex2u8(*hex,&code)) {
            return false;
        }
        val = val << 4;
        val |= code;
    }
    *ret = val;
    return true;
}

bool hex2u16(char * hex,int n,u16 * ret)
{
    u64 val;
    u8  code;
    if (hex[0] == '0' && (hex[1] == 'x' || hex[1] == 'X')) {
        hex += 2;
        n -= 2;
    }
    if (n > 4) {
        return false;
    }
    for (val = 0;n--;hex++) {
        if (!__hex2u8(*hex,&code)) {
            return false;
        }
        val = val << 4;
        val |= code;
    }
    *ret = val;
    return true;
}

bool hex2u32(char * hex,int n,u32 * ret)
{
    u64 val;
    u8  code;
    if (hex[0] == '0' && (hex[1] == 'x' || hex[1] == 'X')) {
        hex += 2;
        n -= 2;
    }
    if (n > 8) {
        return false;
    }
    for (val = 0;n--;hex++) {
        if (!__hex2u8(*hex,&code)) {
            return false;
        }
        val = val << 4;
        val |= code;
    }
    *ret = val;
    return true;
}

bool hex2u64(char * hex,int n,u64 * ret)
{
    u64 val;
    u8  code;
    if (hex[0] == '0' && (hex[1] == 'x' || hex[1] == 'X')) {
        hex += 2;
        n -= 2;
    }
    if (n > 16) {
        return false;
    }
    for (val = 0;n--;hex++) {
        if (!__hex2u8(*hex,&code)) {
            return false;
        }
        val = val << 4;
        val |= code;
    }
    *ret = val;
    return true;
}

bool atomac(char * line,int n,u8 * ret)
{
    if (n != const_strlen("01:34:67:9a:cd:f0")) {
        return false;
    }
    if ((line[2] != ':' || line[5] != ':' || line[8] != ':' || line[11] != ':' || line[14] != ':') &&
        (line[2] != '-' || line[5] != '-' || line[8] != '-' || line[11] != '-' || line[14] != '-')) {
        return false;
    }

    if (!hex2u8(&line[0],2,&ret[0])) {
        return false;
    }
    if (!hex2u8(&line[3],2,&ret[1])) {
        return false;
    }
    if (!hex2u8(&line[6],2,&ret[2])) {
        return false;
    }
    if (!hex2u8(&line[9],2,&ret[3])) {
        return false;
    }
    if (!hex2u8(&line[12],2,&ret[4])) {
        return false;
    }
    if (!hex2u8(&line[15],2,&ret[5])) {
        return false;
    }
    return true;
}

bool atoip(char * line,int n,u32 * ret)
{
    u8 value;
    char * pos;
    char * tail;
    char * start;

    pos = line;
    tail = pos + n;
    *ret = 0;

    start = pos;

    pos = lnindex(start,tail - start,'.');
    if (!pos) {
        return false;
    }
    if (!atou8(start,pos - start,&value)) {
        return false;
    }
    *ret |= value;
    *ret = *ret << 8;

    start = pos + 1;
    pos = lnindex(start,tail - start,'.');
    if (!pos) {
        return false;
    }
    if (!atou8(start,pos - start,&value)) {
        return false;
    }
    *ret |= value;
    *ret = *ret << 8;

    start = pos + 1;
    pos = lnindex(start,tail - start,'.');
    if (!pos) {
        return false;
    }
    if (!atou8(start,pos - start,&value)) {
        return false;
    }
    *ret |= value;
    *ret = *ret << 8;

    start = pos + 1;
    if (!atou8(start,tail - start,&value)) {
        return false;
    }
    *ret |= value;

    return true;
}

bool atoip6(char * line,int n,u128 * ret)
{
    u8 value;
    char * pos;
    char * tail;
    char * start;

    pos = line;
    tail = pos + n;
    *ret = 0;

    start = pos;

    pos = lnindex(start,tail - start,'.');
    if (!pos) {
        return false;
    }
    if (!atou8(start,pos - start,&value)) {
        return false;
    }
    *ret |= value;
    *ret = *ret << 8;

    start = pos + 1;
    pos = lnindex(start,tail - start,'.');
    if (!pos) {
        return false;
    }
    if (!atou8(start,pos - start,&value)) {
        return false;
    }
    *ret |= value;
    *ret = *ret << 8;

    start = pos + 1;
    pos = lnindex(start,tail - start,'.');
    if (!pos) {
        return false;
    }
    if (!atou8(start,pos - start,&value)) {
        return false;
    }
    *ret |= value;
    *ret = *ret << 8;

    start = pos + 1;
    if (!atou8(start,tail - start,&value)) {
        return false;
    }
    *ret |= value;

    return true;
}

bool is_hex_string(const char * string)
{
    while (*string)
    {
        if (!is_hex(*string))
        {
            return false;
        }
        string++;
    }
    return true;
}


