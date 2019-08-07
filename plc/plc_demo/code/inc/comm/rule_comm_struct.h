#ifndef __COMM_IP_PORT_PRO_H__
#define __COMM_IP_PORT_PRO_H__

#define MAX_POLICY_NUM 1

#if 0
typedef struct {
    char    src_ip[64];         ///< 源IP地址，IPv4是点分十进制；IPv6为十六进制
    u16     src_ip_mask;        ///< 源IP地址前缀
    char    dst_ip[64];         ///< 目的IP地址，IPv4是点分十进制；IPv6为十六进制
    u16     dst_ip_mask;        ///< 目的IP地址前缀
    u16     s_src_port;         ///< 起始源端口
    u16     e_src_port;         ///< 结束源端口
    u16     s_dst_port;         ///< 起始目的端口
    u16     e_dst_port;         ///< 结束目的端口
    u8      pro_id;             ///< 协议号
    u8      pro_id_mask;        ///< 协议号掩码
    u8      ext;                ///< 内外层标记位，0内层，1外层
    u8      ip_flag;            ///< 4-IPv4;6-IPv6
}__attribute__((packed)) t_ip_port_pro_info;

typedef struct {
    u32     src_ip;             ///< 源IP地址
    u32     src_ip_mask;        ///< 源IP地址掩码
    u32     dst_ip;             ///< 目的IP地址
    u32     dst_ip_mask;        ///< 目的IP地址掩码
    u16     s_src_port;         ///< 起始源端口
    u16     e_src_port;         ///< 结束源端口
    u16     s_dst_port;         ///< 起始目的端口
    u16     e_dst_port;         ///< 结束目的端口
    u8      pro_id;             ///< 协议号
    u8      pro_id_mask;        ///< 协议号掩码
    u8      ext;                ///< 内外层标记位，0内层，1外层
}__attribute__((packed)) t_ipv4_info;

typedef struct {
    u128    src_ip;             ///< 源IP地址
    u128    src_ip_mask;        ///< 源IP地址掩码
    u128    dst_ip;             ///< 目的IP地址
    u128    dst_ip_mask;        ///< 目的IP地址掩码
    u16     s_src_port;         ///< 起始源端口
    u16     e_src_port;         ///< 结束源端口
    u16     s_dst_port;         ///< 起始目的端口
    u16     e_dst_port;         ///< 结束目的端口
    u8      pro_id;             ///< 协议号
    u8      pro_id_mask;        ///< 协议号掩码
    u8      ext;                ///< 内外层标记位，0内层，1外层
}__attribute__((packed)) t_ipv6_info;

#define     CONDITION_CODE_LEN      64

typedef struct {
    #if __BYTE_ORDER == __LITTLE_ENDIAN
        struct
        {
            u8  offset_type:4;                              ///< 偏移类型 4 bits
            u8  encoded_flag:2;                             ///< 编码方式，0-ASCII码；1-16进制
            u8  regular_flag:1;                             ///< 正则，保留
            u8  float_flag:1;                               ///< 浮动，0-固定偏移规则；1-全包浮动规则
        };
    #else
        struct
        {
            u8  float_flag:1;                               ///< 浮动，0-固定偏移规则；1-全包浮动规则
            u8  regular_flag:1;                             ///< 正则，保留
            u8  encoded_flag:2;                             ///< 编码方式，0-ASCII码；1-16进制
            u8  offset_type:4;                              ///< 偏移类型 4 bits
        };
    #endif
    u8  keep;                                               ///< 保留 1 byte
    u16 offset;                                             ///< 偏移
    u8  condition_code[CONDITION_CODE_LEN];                 ///< 特征码内容
    u8  condition_code_mask[CONDITION_CODE_LEN];            ///< 特征码内容掩码
}__attribute__((packed)) t_condition_code_info;

#define ACCOUNT_LEN 64
typedef struct {
    u8  is_or_not;                          ///< 标记是非规则
    #if __BYTE_ORDER == __LITTLE_ENDIAN
        struct
        {
            u8  keep1:4;
            u8  account_type:4;
        };
    #else
        struct
        {
            u8  account_type:4;
            u8  keep1:4;
        };
    #endif
    u8  keep2;
    u8  account[ACCOUNT_LEN];
    u8  account_mask[ACCOUNT_LEN];
}__attribute__((packed)) t_account_info;

typedef struct {
    u8  format;     ///< 音视频格式
    u8  a_or_v;     ///< A/V
    u8  up_or_down; ///< 音视频上下行
}__attribute__((packed)) t_audio_video_info;

#define HOST_LEN        64
#define URI_LEN         128
#define USER_AGENT_LEN  64
#define COOKIE_LEN      128
#define CONT_TYPE_LEN   32

typedef struct {
    #if __BYTE_ORDER == __LITTLE_ENDIAN
        struct
        {
            u8  keep_b:1;
            u8  ct_flag:1;
            u8  ck_flag:1;
            u8  ua_flag:1;
            u8  u_r:1;
            u8  uri_flag:1;
            u8  h_flag:1;
            u8  m_flag:1;
        };
    #else
        struct
        {
            u8  m_flag:1;
            u8  h_flag:1;
            u8  uri_flag:1;
            u8  u_r:1;
            u8  ua_flag:1;
            u8  ck_flag:1;
            u8  ct_flag:1;
            u8  keep_b:1;
        };
    #endif
    u8  keep1;
    u8  method;
    u8  keep2;
    u8  host[HOST_LEN];
    u8  host_mask[HOST_LEN];
    u8  uri[URI_LEN];
    u8  uri_mask[URI_LEN];
    u8  user_agent[USER_AGENT_LEN];
    u8  cookie[COOKIE_LEN];
    u8  content_type[CONT_TYPE_LEN];
    u8  tlv_t[64];
    u8  tlv_l[64];
    u8  tlv_v[256];
    u8  flag;                   ///< 标记当前规则是否有效
}__attribute__((packed)) t_http_info;

#define DNS_LEN     64

typedef struct {
    u8  dns[DNS_LEN];           ///< DNS域名
    u8  dns_mask[DNS_LEN];      ///< DNS域名掩码
}__attribute__((packed)) t_dns_info;

typedef struct {
    u64 app_id;
    u8  pro_name[16];
}__attribute__((packed)) t_pro_app_info;

#define PHONE_NUMBER_LEN        11
#define LONGITUDE_LATITUDE_LEN  32
#define MAX_PHONE_KEYWORD_LEN   256

extern char g_phone_keywords[MAX_PHONE_KEYWORD_LEN];

typedef struct
{
    u8  phone_number[PHONE_NUMBER_LEN];         ///< 手机号
    u8  phone_number_mask[PHONE_NUMBER_LEN];    ///< 手机号掩码
}__attribute__((packed)) t_phone_info;
typedef struct
{
    u8  s_longitude[LONGITUDE_LATITUDE_LEN];    ///< 起始经度
    u8  e_longitude[LONGITUDE_LATITUDE_LEN];    ///< 结束经度
    u8  s_latitude[LONGITUDE_LATITUDE_LEN];     ///< 起始纬度
    u8  e_latitude[LONGITUDE_LATITUDE_LEN];     ///< 结束纬度
}__attribute__((packed)) t_gps_info;
///< 模型规则，包含手机号和经纬度
///< 暂时只支持11位国内手机号
typedef struct
{
    u8  flag;                                   ///< 标记位，1表示手机号，2表示经纬度，12表示手机号+经纬度，21表示经纬度+手机号
    t_phone_info    phone;                      ///< 手机号信息
    t_gps_info      gps;                        ///< 经纬度信息
}__attribute__((packed)) t_model_info;
#endif

typedef struct
{
    struct xlist_head   list;
    u64                 app_id;
    u8                  host[256];
    u8                  uri[256];
    u8                  user_agent[256];
}t_appid_info;

#define     MAX_HTTP_KEY_WORDS_LEN      64      ///< key words最大长度
#define     MAX_HTTP_KEY_WORDS_NUM      10      ///< key words最大数量
typedef struct
{
    u8      id;
    char    key_word[MAX_HTTP_KEY_WORDS_LEN];
}t_http_key_word;

typedef struct
{
    int                 num;
    t_http_key_word     key_words[MAX_HTTP_KEY_WORDS_NUM];
}t_http_key_words;

extern t_http_key_words key_words;

// 将以";"分割的id分隔开
static inline
u32 id_str_to_int(char *str, u64 *id)
{
    int i, cur, len;
    int count = 0;
    if(!str[0])
    {
        return 0;
    }
    cur = 0;
    len = strlen(str);
    for(i=0; i<len; i++)
    {
        if( ';' == str[i])
        {
            str[i] = '\0';
            id[count] = atoll(str+cur);
            count++;
            cur = i + 1;
        }
    }
    id[count] = atoll(str+cur);
    return ++count;
}

static inline
u32 str_to_val(u8 *val, char *str)
{
    int str_len    = strlen((char *)str);
    int val_idx    = 0;
    int cur_len    = 0;

    val[0] = 0;
    while(cur_len < str_len)
    {
        if(','!= str[cur_len])
        {
            val[val_idx] = val[val_idx] * 10 + str[cur_len] - '0';
        }
        else
        {
            val_idx++;
            val[val_idx] = 0;
        }
        cur_len++;
    }
    return val_idx + 1;
}

#endif
