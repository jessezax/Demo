#ifndef __PM_MSG_HANDLE_H__
#define __PM_MSG_HANDLE_H__

#define PLC_DATA_LEN            32
#define PLC_TIME_LEN            32
#define PLC_IP_LEN              64
#define PLC_COND_CODE_LEN       64
#define PLC_ACCOUNT_LEN         64
#define PLC_HOST_LEN            64
#define PLC_URI_LEN             128
#define PLC_USER_AGENT_LEN      64
#define PLC_COOKIE_LEN          128
#define PLC_CONT_TYPE_LEN       32
#define PLC_DNS_LEN             64
#define PLC_PRO_NAME_LEN        16
#define PLC_PHONE_LEN           11
#define PLC_LONG_LAT_LEN        32

typedef enum
{
    rule_file_type_comm     = 0x00,
    rule_file_type_flow     = 0x01
}e_rule_file_type;

typedef enum
{
    all_trafic              = 0x00,         ///< 上下行流量
    up_trafic               = 0x01,         ///< 上行流量
    down_trafic             = 0x02,         ///< 下行流量
}e_up_down_type;

///< 处理的消息类型，内容Type
typedef enum
{
    handle_msg_ipv4             = 0x00,     ///< IPv4五元组规则
    handle_msg_ipv6             = 0x01,     ///< IPv6五元组规则
    handle_msg_cond_code        = 0x02,     ///< 特征码规则
    handle_msg_account          = 0x03,     ///< 宽带账号规则
    handle_msg_pro_app          = 0x04,     ///< 协议和应用规则
    handle_msg_audio_video      = 0x05,     ///< 音频和视频规则
    handle_msg_http             = 0x06,     ///< HTTP规则
    handle_msg_dns              = 0x07,     ///< DNS域名规则
    handle_msg_model            = 0x08,     ///< 模型规则
    handle_msg_combination      = 0x09,     ///< 组合规则
    handle_msg_type_num,
}e_handle_msg_type;

///< 处理的动作类型,动作Type
typedef enum
{
    action_add                  = 0x00,     ///< 新增操作
    action_del                  = 0x01,     ///< 删除操作
    action_inq                  = 0x02,     ///< 查询操作
    action_num
}e_action_type;

///< 是否是组合规则
typedef enum
{
    not_cmb_rule                = 0x00,     ///< 不是组合规则
    is_cmb_rule                 = 0x01,     ///< 是组合规则
}e_cmb_rule_flag;


typedef enum
{
    model_filed_phone_type      = 0x01,
    model_filed_lang_lat_type   = 0x02,
}e_model_filed_type;

/* 协议和应用文件读取信息存储类型 */
typedef enum
{
    pro_app_url                 = 0x00,     ///< URL
    pro_app_uri                 = 0x01,     ///< URI
    pro_app_host                = 0x02,     ///< HOST
    pro_app_user_agent          = 0x03,     ///< USER_AGENT
    pro_app_type_num
}e_pro_app_type;

/* 协议和应用文件读取信息存储 */
typedef struct
{
    struct  xlist_head  list_head;
    u64                 app_id;
    e_pro_app_type      type;
    u32                 uri_len;            ///< 记录uri的长度,仅仅对URL有效
    u32                 data_len;           ///< 记录数据部分的长度,如果是URL,则是完整URL长度
    char                data[0];
}__attribute__((packed)) t_pro_app_type_info;

/* 处理的消息结构体 */
typedef struct
{
    struct  xlist_head  list_head;
    u64                 hit_counts;         ///< 命中次数
    char                date[PLC_DATA_LEN]; ///< 日期,时间
    //char              time[PLC_TIME_LEN]; ///< 时间
    char                src_ip[PLC_IP_LEN]; ///< 源IP地址
    u16                 src_port;           ///< 源端口
    u8                  ip_flag;            ///< 4-IPv4, 6-IPv6
    e_up_down_type      up_or_down;         ///< 上行还是下行
    e_rule_file_type    type;               ///< 标记是否需要生成flow.payload规则
    e_comm_msg_sub_type sub_type;           ///< 策略类型
    u32                 data_len;           ///< 数据部分的长度
    char                data[0];            ///< 数据部分,数据部分具体的内容采用TLV的格式, 动作TLV+内容TLV
}__attribute__((packed)) t_handle_msg_info;

/* 动作 */
/* 新增操作结构体 */
typedef struct
{
    u8                  user_id;            ///< 用户ID
    u8                  port_group;         ///< 输出端口组号
    #if __BYTE_ORDER == __LITTLE_ENDIAN
        struct
        {
            u8  keep:3;                     ///< 保留
            u8  single:1;                   ///< 单包匹配标记位, 0-跨包 1-单包
            u8  hst:1;                      ///< 中标统计, 0-不统计; 1-统计
            u8  save:1;                     ///< 重启保存, 0-不保存; 1-保存
            u8  act:2;                      ///< 动作, 0-丢弃, 1-转发
        };
    #else
        struct
        {
            u8  act:2;                      ///< 动作, 0-丢弃, 1-转发
            u8  save:1;                     ///< 重启保存, 0-不保存; 1-保存
            u8  hst:1;                      ///< 中标统计, 0-不统计; 1-统计
            u8  single:1;                   ///< 单包匹配标记位, 0-跨包 1-单包
            u8  keep:3;                     ///< 保留
        };
    #endif
    u8                  isp;                ///< 运营商 默认0x00
    u32                 rule_id;            ///< 规则ID
    u32                 timestamp;          ///< 时间戳
    u8                  type[4];            ///< 类型,组合规则2，3有效
}__attribute__((packed)) t_action_add;

/* 删除操作结构体 */
typedef struct
{
    u8                  user_id;            ///< 用户ID
    u8                  isp;                ///< 运营商 默认0x00
    u32                 rule_id;            ///< 规则ID
}__attribute__((packed)) t_action_del;

/* 查询操作结构体 */
typedef struct
{
    u8                  user_id;            ///< 用户ID
    u8                  keep;               ///< 保留
    u32                 rule_id;            ///< 规则ID
    u64                 hit_count;          ///< 名中次数
}__attribute__((packed)) t_action_inq;
/* 动作结束 */

/* 数据 */
/* IPv4 */
typedef struct
{
    u32     src_ip;                         ///< 源IP地址
    u32     src_ip_mask;                    ///< 源IP地址掩码
    u32     dst_ip;                         ///< 目的IP地址
    u32     dst_ip_mask;                    ///< 目的IP地址掩码
}__attribute__((packed)) t_ipv4_info;

/* IPv6 */
typedef struct {
    u128    src_ip;                         ///< 源IP地址
    u128    src_ip_mask;                    ///< 源IP地址掩码
    u128    dst_ip;                         ///< 目的IP地址
    u128    dst_ip_mask;                    ///< 目的IP地址掩码
}__attribute__((packed)) t_ipv6_info;

/* 五元组 */
typedef struct
{
    union
    {
        t_ipv4_info ipv4_info;
        t_ipv6_info ipv6_info;
    };
    u16     s_src_port;                     ///< 起始源端口
    u16     e_src_port;                     ///< 结束源端口
    u16     s_dst_port;                     ///< 起始目的端口
    u16     e_dst_port;                     ///< 结束目的端口
    u8      pro_id;                         ///< 协议号
    u8      pro_id_mask;                    ///< 协议号掩码
    u8      ext;                            ///< 内外层标记位，0内层，1外层
}__attribute__((packed)) t_ip_port_pro_info;

/* 特征码 */
typedef struct {
    #if __BYTE_ORDER == __LITTLE_ENDIAN
        struct
        {
            u8  offset_type:4;                      ///< 偏移类型 4 bits
            u8  encoded_flag:2;                     ///< 编码方式，0-ASCII码；1-16进制
            u8  regular_flag:1;                     ///< 正则，保留
            u8  float_flag:1;                       ///< 浮动，0-固定偏移规则；1-全包浮动规则
        };
    #else
        struct
        {
            u8  float_flag:1;                       ///< 浮动，0-固定偏移规则；1-全包浮动规则
            u8  regular_flag:1;                     ///< 正则，保留
            u8  encoded_flag:2;                     ///< 编码方式，0-ASCII码；1-16进制
            u8  offset_type:4;                      ///< 偏移类型 4 bits
        };
    #endif
    u8      keep;                                   ///< 保留 1 byte
    u16     offset;                                 ///< 偏移
    char    condition_code[PLC_COND_CODE_LEN];      ///< 特征码内容
    u8      condition_code_mask[PLC_COND_CODE_LEN]; ///< 特征码内容掩码
}__attribute__((packed)) t_cond_code_info;

/* 账号 */
typedef struct {
    #if __BYTE_ORDER == __LITTLE_ENDIAN
        struct
        {
            u8  keep1:4;
            u8  account_type:4;                 ///< 账号类型
    };
    #else
        struct
        {
            u8  account_type:4;                 ///< 账号类型
            u8  keep1:4;
        };
    #endif
    u8      keep2;
    char    account[PLC_ACCOUNT_LEN];           ///< 账号
    u8      account_mask[PLC_ACCOUNT_LEN];      ///< 账号掩码
}__attribute__((packed)) t_account_info;

typedef struct {
    u8  format;                                 ///< 音视频格式
    u8  a_or_v;                                 ///< A/V
    u8  up_or_down;                             ///< 音视频上下行
}__attribute__((packed)) t_audio_video_info;

/* HTTP */
typedef struct {
    #if __BYTE_ORDER == __LITTLE_ENDIAN
        struct
        {
            u8  keep_b:1;                       ///< 保留
            u8  ct_flag:1;                      ///< 0-content-type字段无效; 1-content-type字段有效
            u8  ck_flag:1;                      ///< 0-cookie字段无效; 1-cookie字段有效
            u8  ua_flag:1;                      ///< 0-user agent字段无效; 1-user agent字段有效
            u8  u_r:1;                          ///< 保留
            u8  uri_flag:1;                     ///< 0-URI字段无效; 1-URI字段有效
            u8  h_flag:1;                       ///< 0-HOST字段无效; 1-HOST字段有效
            u8  m_flag:1;                       ///< 0-method字段无效; 1-method字段有效
        };
    #else
        struct
        {
            u8  m_flag:1;                       ///< 0-method字段无效; 1-method字段有效
            u8  h_flag:1;                       ///< 0-HOST字段无效; 1-HOST字段有效
            u8  uri_flag:1;                     ///< 0-URI字段无效; 1-URI字段有效
            u8  u_r:1;                          ///< 保留
            u8  ua_flag:1;                      ///< 0-user agent字段无效; 1-user agent字段有效
            u8  ck_flag:1;                      ///< 0-cookie字段无效; 1-cookie字段有效
            u8  ct_flag:1;                      ///< 0-content-type字段无效; 1-content-type字段有效
            u8  keep_b:1;                       ///< 保留
        };
    #endif
    u8      keep1;                              ///< 保留
    u8      method;                             ///< Method类型
    u8      keep2;                              ///< 保留
    char    host[PLC_HOST_LEN];                 ///< Host
    u8      host_mask[PLC_HOST_LEN];            ///< Host掩码
    char    uri[PLC_URI_LEN];                   ///< URI
    u8      uri_mask[PLC_URI_LEN];              ///< URI掩码
    char    user_agent[PLC_USER_AGENT_LEN];     ///< User-Agent
    char    cookie[PLC_COOKIE_LEN];             ///< COOKIE
    char    content_type[PLC_CONT_TYPE_LEN];    ///< Content-Type
    u8      tlv[256];                           ///< 扩展字段
}__attribute__((packed)) t_http_info;

/* DNS */
typedef struct {
    char    dns[PLC_DNS_LEN];                   ///< DNS域名
    u8      dns_mask[PLC_DNS_LEN];              ///< DNS域名掩码
}__attribute__((packed)) t_dns_info;

/* 协议和应用ID */
typedef struct {
    u64     app_id;                             ///< 应用ID
    char    pro_name[PLC_PRO_NAME_LEN];         ///< 协议名称
}__attribute__((packed)) t_pro_app_info;

/* 模型规则 */
typedef struct
{
    char    phone_number[PLC_PHONE_LEN];        ///< 手机号
    u8      phone_number_mask[PLC_PHONE_LEN];   //< 手机号掩码
}__attribute__((packed)) t_phone_info;

typedef struct
{
    char    s_longitude[PLC_LONG_LAT_LEN];      ///< 起始经度
    char    e_longitude[PLC_LONG_LAT_LEN];      ///< 结束经度
    char    s_latitude[PLC_LONG_LAT_LEN];       ///< 起始纬度
    char    e_latitude[PLC_LONG_LAT_LEN];       ///< 结束纬度
}__attribute__((packed)) t_gps_info;

///< 模型规则，
///< 包含手机号和经纬度
///< 暂时只支持11位国内手机号
typedef struct
{
    u8              flag;                       ///< 标记位, 1表示手机号, 2表示经纬度, 12表示手机号+经纬度, 21表示经纬度+手机号
    t_phone_info    phone;                      ///< 手机号信息
    t_gps_info      gps;                        ///< 经纬度信息
}__attribute__((packed)) t_model_info;

/*
 * #########################################################################
 */

/**
 * @brief                   协议和应用ID信息加载
 * @return                  bool
 */
bool comm_appid_info_load(void);

/**
 * @brief                   对十六进制字符串和掩码进行有效性校验
 * @param[in]   str         十六进制字符串
 * @param[in]   mask        掩码
 * @param[in]   len         数据长度
 * @return                  bool
 */
bool legality_check_for_hex(char *hex, u8 *mask, int len);
/**
 * @brief                   对字符串和掩码进行有效性校验
 * @param[in]   str         字符串
 * @param[in]   mask        掩码
 * @param[in]   len         数据长度
 * @return                  bool
 */
bool legality_check_for_str(char *str, u8 *mask, int len);

/**
 * @brief                   策略处理消息alloc
 * @param[in]   src_ip      源IP
 * @param[in]   port        源端口
 * @param[in]   data_len    数据长度
 * @param[in]   data        数据
 * @return                  t_handle_msg_info
 */
t_handle_msg_info * handle_msg_info_alloc(e_comm_msg_sub_type type, char *src_ip, u16 port, size_t data_len, char *data);
void handle_msg_info_free(t_handle_msg_info * msg);

/**
 * @brief                   策略删除消息
 * @param[in]   conn        连接上下文信息
 * @param[in]   pos         原始消息的起始位置
 * @param[in]   tail        原始消息的结束位置
 * @param[in]   msg_type    处理消息内容的实际类型
 * @param[in]   __rule_id   消息对应的规则ID
 * @param[in]   error_num   错误码
 * @param[in]   arg         扩展
 * @return                  t_handle_msg_info
 */
t_handle_msg_info *load_info_for_del(   t_connection * conn,
                                        u8 * pos,
                                        u8 * tail,
                                        e_handle_msg_type msg_type,
                                        u32 *__rule_id,
                                        e_comm_return_code *error_num,
                                        void *arg);

/**
 * @brief                   策略查询消息
 * @param[in]   conn        连接上下文信息
 * @param[in]   pos         原始消息的起始位置
 * @param[in]   tail        原始消息的结束位置
 * @param[in]   msg_type    处理消息内容的实际类型
 * @param[in]   __rule_id   消息对应的规则ID
 * @param[in]   error_num   错误码
 * @param[in]   arg         扩展
 * @return                  t_action_inq
 */
t_handle_msg_info *load_info_for_inq(   t_connection * conn,
                                        u8 * pos,
                                        u8 * tail,
                                        e_handle_msg_type msg_type,
                                        u32 *__rule_id,
                                        e_comm_return_code *error_num,
                                        void *arg);

#endif
