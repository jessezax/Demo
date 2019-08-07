#ifndef __comm_MSG_H__
#define __comm_MSG_H__

///< 消息版本
#define comm_VERSION (0x01)

#pragma pack(1)
///< 策略报文消息头
typedef struct
{
    u16     msg_id;         ///< 消息标识
    u16     msg_type;       ///< 消息类型代码
    u32     msg_length;     ///< 消息体长度(包括消息头长度)
    u8      data[0];        ///< 策略消息负载起始地址
}t_comm_msg;
#pragma pack()

///< FTP用户名长度
#define FTP_USER_LEN 8
///< FTP密码长度
#define FTP_PASSWD_LEN 8

///< 读取出的数据字节序
enum {
    network_order = 0,
    host_order    = 1
};

///< 判断缓冲区剩余空间,读取指定长度的内容
#define read_content(dst,pos,tail,size) \
do { \
    if (!pre_tail(pos,tail,size)) { \
        goto error; \
    } \
    char *__tmp__ = (char *)dst;\
    if (__tmp__) { \
        memcpy(__tmp__,pos,size); \
    } \
    pos += size;\
} while(0)

///< 读取字符串,字符串长度为1字节
#define read_string(string,pos,tail) \
do { \
    u8   string_len; \
    if (!pre_tail(pos,tail,sizeof(u8))) { \
        goto error; \
    } \
    string_len = *pos; \
    pos++; \
    if (!pre_tail(pos,tail,string_len)) {\
        goto error;\
    }\
    set_string(string, (char *)pos, string_len);\
    pos += string_len;\
} while(0)

///< 读取字符串,字符串长度为2字节
#define read_string2(string,pos,tail) \
do { \
    u16   string_len; \
    if (!pre_tail(pos,tail,sizeof(u16))) { \
        goto error; \
    } \
    string_len = ntohs(*(u16 *)pos); \
    pos+=2; \
    if (!pre_tail(pos,tail,string_len)) {\
        goto error;\
    }\
    set_string(string, (char *)pos, string_len);\
    pos += string_len;\
} while(0)

///< 读取1字节
#define read_u8(dst,pos,tail) \
do { \
    if (!pre_tail(pos,tail,sizeof(u8))) { \
        goto error; \
    } \
    dst = *pos; \
    pos += sizeof(u8); \
}while(0)

///< 读取2字节
#define read_u16(dst,pos,tail,byte_order) \
do { \
    if (!pre_tail(pos,tail,sizeof(u16))) { \
        goto error; \
    } \
    dst = *((u16 *)pos); \
    if (byte_order == host_order) { \
        dst = htons(dst);\
    }\
    pos += sizeof(u16); \
}while(0)

///< 读取4字节
#define read_u32(dst,pos,tail,byte_order) \
do { \
    if (!pre_tail(pos,tail,sizeof(u32))) { \
        goto error; \
    } \
    dst = *((u32 *)pos); \
    if (byte_order == host_order) { \
        dst = ntohl(dst);\
    }\
    pos += sizeof(u32); \
}while(0)

///< 读取8字节
#define read_u64(dst,pos,tail,byte_order) \
do { \
    if (!pre_tail(pos,tail,sizeof(u64))) { \
        goto error; \
    } \
    dst = *((u64 *)pos); \
    if (byte_order == host_order) { \
        dst = ntohll(dst);\
    }\
    pos += sizeof(u64); \
}while(0)

///< 读取16字节
#define read_u128(dst,pos,tail,byte_order) \
do { \
    if (!pre_tail(pos,tail,sizeof(u128))) { \
        goto error; \
    } \
    dst = *((u128 *)pos); \
    if (byte_order == host_order) { \
        dst = ntoh128(dst);\
    }\
    pos += sizeof(u128); \
}while(0)

///< 消息的具体动作
typedef enum {
    comm_msg_action_con             = 0x01, ///< 连接
    comm_msg_action_add             = 0x02, ///< 新增
    comm_msg_action_del             = 0x03, ///< 删除
    comm_msg_action_inq             = 0x04, ///< 查询
}e_comm_msg_action_type;

///< 消息类型代码
typedef enum {
    comm_msg_ip_port_pro_req        = 0x01, ///< 五元组规则请求
    comm_msg_ip_port_pro_res        = 0x02, ///< 五元组规则应答
    comm_msg_cond_code_req          = 0x03, ///< 特征码规则请求
    comm_msg_cond_code_res          = 0x04, ///< 特征码规则应答
    comm_msg_account_req            = 0x05, ///< 宽带账号规则请求
    comm_msg_account_res            = 0x06, ///< 宽带账号规则应答
    comm_msg_pro_app_req            = 0x07, ///< 协议和应用规则请求
    comm_msg_pro_app_res            = 0x08, ///< 协议和应用规则应答
    comm_msg_audio_video_req        = 0x0d, ///< 音频和视频规则请求
    comm_msg_audio_video_res        = 0x0e, ///< 音频和视频规则应答
    comm_msg_http_req               = 0x0f, ///< HTTP规则请求
    comm_msg_http_res               = 0x11, ///< HTTP规则应答
    comm_msg_dns_req                = 0x12, ///< DNS域名规则请求
    comm_msg_dns_res                = 0x13, ///< DNS域名规则应答
    comm_msg_model_req              = 0x14, ///< 模型规则请求
    comm_msg_model_res              = 0x15, ///< 模型规则应答
    comm_msg_data_flow_req          = 0x16, ///< 数据量前N包规则请求
    comm_msg_data_flow_res          = 0x17, ///< 数据量前N包规则应答
    comm_msg_combination_req        = 0xfe, ///< 组合规则请求
    comm_msg_combination_res        = 0xff, ///< 组合规则应答
}e_comm_msg_type;

//< 子消息类型
typedef enum{
    comm_sub_msg_ipv4               = 0x01, ///< IPv4五元组
    comm_sub_msg_ipv6               = 0x02, ///< IPv6五元组
    comm_sub_msg_condi_code         = 0x03, ///< 特征码
    comm_sub_msg_account            = 0x04, ///< 宽带账号
    comm_sub_msg_pro_app            = 0x05, ///< 协议和应用
    comm_sub_msg_audio_video        = 0x06, ///< 音频和视频
    comm_sub_msg_http               = 0x07, ///< HTTP
    comm_sub_msg_dns                = 0x08, ///< DNS
    comm_sub_msg_model              = 0x09, ///< 模型
    comm_sub_msg_cmb                = 0x0a, ///< 组合

    comm_max_sub_msg
}e_comm_msg_sub_type;

///< 移动消息错误状态
typedef enum
{
    PM_RET_SUCCESS                  = 0x0000,       ///< 操作成功
    PM_RET_RULE_FULL                = 0x0001,       ///< 规则已满
    PM_RET_RULE_EXIST               = 0x0002,       ///< 规则已存在
    PM_RET_RULE_NO_EXIST            = 0x0003,       ///< 规则不存在
    PM_RET_INVALID_ID               = 0x0004,       ///< 无效规则ID
    PM_RET_INVALID_COMMAND          = 0x0005,       ///< 无效的指令参数
    PM_RET_INVALID_MSG_TYPE         = 0x0006,       ///< 无效的消息类型
    PM_RET_INVALID_USER             = 0x0007,       ///< 无效的用户
    PM_RET_INVALID_PASSWD           = 0x0008,       ///< 无效的口令
    PM_RET_NOT_COMMPLETED_REQ       = 0x0009,       ///< 当前请求未完成
    PM_RET_NOT_COMMPLETED_HOLD      = 0x000a,       ///< 当前保存为完成
    PM_RET_BUSY_SERVER              = 0x000b,       ///< 服务器忙
    PM_RET_UNKNOWN_ERROR            = 0x000c        ///< 未知错误
}e_comm_return_code;


/**
 * @brief                   策略消息处理函数类型
 * @param[in]  conn         策略消息连接
 * @param[in]  msg_id       策略消息ID
 * @param[in]  msg_version  策略消息版本号
 * @param[in]  pos          策略消息负载起始地址
 * @param[in]  tail         策略消息负载结束地址
 * @param[in]  arg          回调参数
 * @return                  bool
 */
typedef bool (*f_comm_msg_handle)(t_connection * conn,u16 msg_id,e_comm_msg_action_type action,u8 * pos,u8 * tail,void * arg);

/**
 * @brief                   策略消息处理注册函数
 * @param[in]  name         策略消息名称
 * @param[in]  msg_type     策略消息类型
 * @param[in]  handle       策略消息处理函数
 * @return                  bool
 */
bool comm_msg_handler_register(const char * name,u8 msg_type,f_comm_msg_handle handle);

/**
 * @brief                   策略消息处理函数
 * @param[in]  conn         策略消息连接
 * @param[in]  msg_type     策略消息类型
 * @param[in]  msg_action   策略消息动作
 * @param[in]  msg_id       策略消息ID
 * @param[in]  pos          策略消息负载起始地址
 * @param[in]  tail         策略消息负载结束地址
 * @param[in]  arg          回调参数
 * @return                  void
 */
void __comm_msg_handle(t_connection * conn,u8 msg_type,e_comm_msg_action_type msg_action,u16 msg_id,u8 * pos,u8 * tail,void * args);

/**
 * @brief                   策略消息处理函数
 * @param[in]  conn         策略消息连接
 * @param[in]  msg          策略消息结构
 * @param[in]  arg          回调参数
 * @return                  void
 */
void comm_msg_handle(t_connection * conn,t_comm_msg * msg,void * args);

///< 消息头类型
typedef enum {
    mh_msg_type,    ///< 消息类型
    mh_msg_id,      ///< 消息ID
}e_comm_header;

/**
 * @brief                   申请消息结构体
 * @param[in]  append_max   策略消息负载最大空间
 * @return                  消息结构指针
 */
t_comm_msg * comm_msg_alloc(u32 append_max);

/**
 * @brief                   释放消息结构体
 * @param[in]  msg          消息结构指针
 * @return                  void
 */
void comm_msg_free(t_comm_msg * msg);

/**
 * @brief                   释放消息结构体
 * @param[in]  msg          消息结构指针
 * @return                  void
 */
bool comm_msg_set_header(t_comm_msg * msg,e_comm_header header_idx,u32 data);
bool comm_msg_append(t_comm_msg * msg,void * data,u32 size);
bool comm_msg_append_u8(t_comm_msg * msg,u8 val);
bool comm_msg_append_u16(t_comm_msg * msg,u16 val,u8 byte_order);
bool comm_msg_append_u32(t_comm_msg * msg,u32 val,u8 byte_order);
#if 1
bool comm_msg_append_u64(t_comm_msg * msg,u64 val,u8 byte_order);
#endif
bool __comm_msg_append_string(t_comm_msg * msg,char * val,u32 size);
bool comm_msg_append_string(t_comm_msg * msg,t_string * string);
bool __comm_msg_append_string2(t_comm_msg * msg,char * val,u32 size);
bool comm_msg_append_string2(t_comm_msg * msg,t_string * string);


bool comm_msg_recv(t_connection * conn,t_comm_msg ** msg);
bool comm_msg_send(t_connection * conn,t_comm_msg * msg);

#endif
