#include "libx.h"
#include "comm.h"

bool legality_check_for_hex(char *hex, u8 *mask, int len)
{
    int     idx;
    bool    ff_flag     = false;                ///< 掩码是否含有0xff

    for (idx = 0; idx < len / 2; idx++)
    {
        if (mask[2 * idx] != mask[2 * idx + 1])
        {
            return false;
        }

        if ((0xff != mask[2 *idx]) && (0x00 != mask[2 *idx]))
        {
            return false;
        }
    }

    for (idx = 0; idx < len; idx++)
    {
        if (0xff == mask[idx])
        {
            if (!hex[idx])
            {
                return false;
            }
            ff_flag = true;
        }
    }

    return ff_flag;
}

bool legality_check_for_str(char *str, u8 *mask, int len)
{
    int     idx;                                ///< for循环索引
    bool    ff_flag     = false;                ///< 掩码是否含有0xff

    for (idx = 0; idx < len; idx++)
    {
        if (0xff == mask[idx])
        {
            if (!str[idx])
            {
                return false;
            }
            ff_flag = true;
        }
        else if (0x0 != mask[idx])
        {
            return false;
        }
    }

    return ff_flag;
}

void handle_msg_info_free(t_handle_msg_info * msg)
{
    xlist_del(&msg->list_head);
    x_free(msg);
}

/**
 * @brief                   策略处理消息alloc
 * @param[in]   src_ip      源IP
 * @param[in]   port        源端口
 * @param[in]   data_len    数据长度
 * @param[in]   data        数据
 * @return                  t_handle_msg_info
 */
t_handle_msg_info * handle_msg_info_alloc(e_comm_msg_sub_type type, char *src_ip,u16 port,size_t data_len,char *data)
{
    time_t t;
    struct tm * lt;

    t_handle_msg_info *msg = x_zalloc(sizeof(t_handle_msg_info) + data_len + 1);
    if (!msg)
    {
        return NULL;
    }

    time (&t);                                  ///< 获取Unix时间戳。
    lt = localtime (&t);                        ///< 转为时间结构。

    snprintf(msg->date, PLC_DATA_LEN, "%d-%d-%d %d:%d:%d",
                                            lt->tm_year+1900, lt->tm_mon, lt->tm_mday,
                                            lt->tm_hour, lt->tm_min, lt->tm_sec);
    //snprintf(msg->time, PLC_TIME_LEN, "%d:%d:%d", lt->tm_hour, lt->tm_min, lt->tm_sec);

    if(src_ip)
    {
        snprintf(msg->src_ip, PLC_IP_LEN, "%s", src_ip);
    }

    msg->src_port = port;
    msg->data_len = data_len;
    msg->sub_type = type;

    memcpy(msg->data, data, msg->data_len);

    return msg;
}

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
                                        void *arg)
{
    t_action_del        del_info;                           ///< 删除信息
    t_handle_msg_info   *msg                = NULL;         ///< 新增消息完整结构

    memset(&del_info, 0x0, sizeof(t_action_del));           ///< 删除消息初始化

    read_u8(del_info.user_id, pos, tail);                   ///< 读取用户ID
    read_u8(del_info.isp, pos, tail);                       ///< 读取运营商信息
    read_u32(del_info.rule_id, pos, tail, host_order);      ///< 读取规则ＩＤ

    *__rule_id = del_info.rule_id;

    if (del_info.user_id != conn->user_id)                  ///< 判断user_id是否一致
    {
        goto user_error;
    }

    msg = pm_plc_data_del(del_info.user_id, del_info.rule_id, msg_type);
    if (!msg)
    {
        goto no_exist;
    }

    /* 获取远程服务器的IP和端口信息 */
    if(4 == conn->flag) ///< IPv4
    {
        inet_ntop(AF_INET, &conn->remote_addr.sin_addr, msg->src_ip, PLC_IP_LEN);
        msg->src_port = ntohs(conn->remote_addr.sin_port);
    }
    else    ///< IPv6
    {
        inet_ntop(AF_INET6, &conn->remote_addr_ipv6.sin6_addr, msg->src_ip, PLC_IP_LEN);
        msg->src_port = ntohs(conn->remote_addr_ipv6.sin6_port);
    }

    *error_num = PM_RET_SUCCESS;
    return msg;

user_error:
    *error_num  = PM_RET_INVALID_USER;
    return NULL;
no_exist:
    *error_num = PM_RET_RULE_NO_EXIST;
    return NULL;
error:
    *error_num = PM_RET_INVALID_ID;
    return NULL;
}

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
                                        void *arg)
{
    char            hash_data[16]   = {0};              ///< 用于存储哈希数据
    t_action_inq    inq_info;                           ///< 查询请求信息
    t_handle_msg_info   *handle_msg = NULL;

    memset(&inq_info, 0x0, sizeof(t_action_inq));

    read_u8(inq_info.user_id, pos, tail);               ///< 读取user_id信息
    read_u8(inq_info.keep, pos, tail);                  ///< 读取保留位
    read_u32(inq_info.rule_id, pos, tail, host_order);  ///< 读取规则ID

    *__rule_id = inq_info.rule_id;

    /* 判断user_id是否一致 */
    if (inq_info.user_id != conn->user_id)
    {
        goto user_error;
    }

    /* 查找数据是否存在 */
    /* 构建哈希数据 */
    hash_data[0] = inq_info.user_id;
    memcpy(hash_data+1, &inq_info.rule_id, sizeof(u32));

    /* 哈希查找, 未找到,       返回错误 */
    handle_msg = pm_plc_data_search(inq_info.user_id, inq_info.rule_id);
    if (!handle_msg)
    {
        goto no_exist;
    }

    /* 发送消息给DPI,     获取查询命中次数 */
    handle_msg->hit_counts = inq_notify(inq_info.user_id, inq_info.rule_id);

    *error_num = PM_RET_SUCCESS;

    return handle_msg;

user_error:
    *error_num  = PM_RET_INVALID_USER;
    return NULL;
no_exist:
    *error_num = PM_RET_RULE_NO_EXIST;
    return NULL;
error:
    *error_num = PM_RET_UNKNOWN_ERROR;
    return NULL;
}

