/**
 * @file       comm_string.h
 * @brief      字符串接口
 * @version    1.0.0.1
 * @author     郭志龙
 * @email      guozhl@sugon.com
 * @date       2018-09-25
 * @license    
 * @details    
 * @changelog  
 * +-------------+-----------+----------------+---------------------------------+
 * |  <Date>     | <Version> | <Author>       | <Description>                   |
 * +-------------+----------------------------+---------------------------------+
 * |  2018-09-25 | 1.0.0.1   | 郭志龙            | Create file                     |
 * +-------------+-----------+----------------+---------------------------------+
 *
 */

#ifndef __X_STRING_H__
#define __X_STRING_H__

/**
 * @addtogroup libx
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief 字符串结构
 */
typedef struct {
    unsigned int dup:1;     ///< 是否使用dup接口动态生成
    unsigned int len:31;    ///< 字符串长度
    char *       data;      ///< 字符串指针
}t_string;

#define const_strlen(str) (sizeof(str) - 1)

/**
 * @brief                   静态字符串实例
 * @param[in]  str          静态字符串
 */
#define const_string(str)     {0,const_strlen(str),(char*)str}

/**
 * @brief                   空字符串实例
 */
#define null_string           {0,0,NULL}

/**
 * @brief                   设置字符串
 * @param[in]  p            t_string 指针
 * @param[in]  text         字符串指针 char*
 * @param[in]  length       字符串长度
 */
#define set_string(p, text, length)   do{(p)->len = length; (p)->data = (text);(p)->dup = 0;}while (0)

/**
 * @brief                   设置静态字符串
 * @param[in]  p            指针 t_string*
 * @param[in]  text         静态字符串指针 cosnt char*
 */
#define set_const_string(p, text) set_string(p,text,const_strlen(text))

/**
 * @brief                   设置空字符串
 * @param[in]  p            指针 t_string*
 */
#define set_null_string(p)   set_string(p,NULL,0)

/**
 * @brief                   复制字符串(深拷贝)
 * @param[in]  src          源字符串指针
 * @param[in]  len          源字符串长度
 * @param[in]  dst          目的t_string指针
 * @return                  0表示失败，1表示成功
 */
unsigned char __string_dup(const char *src,unsigned int len,t_string * dst);

/**
 * @brief                   复制字符串(深拷贝)
 * @param[in]  src          源t_string指针
 * @param[in]  dst          目的t_string指针
 * @return                  0表示失败，1表示成功
 */
unsigned char string_dup(t_string * src,t_string * dst);

/**
 * @brief                   释放复制的字符串
 * @param[in]  p            待释放t_string指针
 * @note                    非p->dup=0时不执行free操作
 */
void string_free(t_string * p);

/**
 * @brief                   字符串浅拷贝
 * @param[in]  src          源t_string指针
 * @param[in]  dst          目的t_string指针
 * @note                    如果dst->dup=1，执行成功后dst->dup=0,src->dup=1
 */
static inline
void string_copy(t_string * src,t_string * dst)
{
    *dst = *src;
    if (src->dup) {
        src->dup = 0;
    }
}

//left strlen index
char * lnindex(char * s,u32 n, char c);

bool atob(const char *line, int n,bool * ret);

//success return ture,failure return false;
bool atou8(const char *line, int n,u8 * ret);
//success return ture,failure return false;
bool atou16(const char *line, int n,u16 * ret);
//success return ture,failure return false;
bool atou32(const char *line, int n,u32 * ret);
//success return ture,failure return false;
bool atou64(const char *line, int n,u64 * ret);
//success return ture,failure return false;
bool atos8(const char *line, int n,s8 * ret);
//success return ture,failure return false;
bool atos16(const char *line, int n,s16 * ret);
//success return ture,failure return false;
bool atos32(const char *line, int n,s32 * ret);
//success return ture,failure return false;
bool atos64(const char *line, int n,s64 * ret);

//success return ture,failure return false;
bool atosz(const char *line, int n,ssize_t * ret);
//success return ture,failure return false;
bool atoof(const char *line, int n,off_t * ret);
//success return ture,failure return false;
bool atotm(const char *line, int n,time_t * ret);

bool __hex2u8(char hex,u8 * ret);
bool hex2u8(char * hex,int n,u8 * ret);
bool hex2u16(char * hex,int n,u16 * ret);
bool hex2u32(char * hex,int n,u32 * ret);
bool hex2u64(char * hex,int n,u64 * ret);

bool atomac(char * line,int n,u8 * ret);
bool atoip(char * line,int n,u32 * ret);
bool atoip6(char * line,int n,u128 * ret);

bool is_hex_string(const char * string);


#define is_num(c) ((c) >= '0' && (c) <= '9')
#define is_hex(c) (((c) >= '0' && (c) <= '9') || ((c) >= 'a' && (c) <= 'f') || ((c) >= 'A' && (c) <= 'F'))

static inline bool match_charlist(char c,const char * list,int list_len)
{
    int index;
    for (index = 0;index < list_len;index++)
    {
        if (c == list[index])
        {
            return true;
        }
    }
    return false;
}

static inline int __ltrim(char * string,int string_len,const char * charlist,int charlist_len)
{
    char * pos;
    char * string_end;

    string_end = string + string_len;
    pos = string;

    while (pos < string_end)
    {
        if (!match_charlist(*pos,charlist,charlist_len))
        {
            break;
        }
        pos++;
    }
    if (pos > string)
    {
        memmove(string,pos,string_len - (pos - string) + 1);
    }
    return string_len - (pos - string);
}

static inline int ltrim(char * string,const char * charlist)
{
    int string_len;
    int charlist_len;
    
    string_len = strlen(string);
    charlist_len = strlen(charlist);
    return __ltrim(string,string_len,charlist,charlist_len);
}

static inline int __rtrim(char * string,int string_len,const char * charlist,int charlist_len)
{
    char * pos;
    char * string_end;
    string_end = string + string_len;
    pos = string_end - 1;

    while (pos >= string)
    {
        if (!match_charlist(*pos,charlist,charlist_len))
        {
            break;
        }
        pos--;
    }
    pos++;
    if (pos < string_end)
    {
        *pos = 0;
    }
    return string_len - (string_end - pos);
}

static inline int rtrim(char * string,const char * charlist)
{
    int string_len;
    int charlist_len;
    
    string_len = strlen(string);
    charlist_len = strlen(charlist);
    return __rtrim(string,string_len,charlist,charlist_len);
}

static inline int __trim(char * string,int string_len,const char * charlist,int charlist_len)
{
    string_len = __rtrim(string,string_len,charlist,charlist_len);
    if (string_len > 0)
    {
        return __ltrim(string,string_len,charlist,charlist_len);
    }
    return string_len;
}

static inline int trim(char * string,const char * charlist)
{
    int string_len;
    int charlist_len;
    
    string_len = strlen(string);
    charlist_len = strlen(charlist);

    return __trim(string,string_len,charlist,charlist_len);
}

#ifdef __cplusplus
}
#endif

/*! @} */

#endif

