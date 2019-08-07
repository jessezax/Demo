#include "libx.h"
#include "comm.h"

#define LOG_SRT_LEN 1024

void plc_log(   t_connection * conn,
                t_handle_msg_info *msg_info,
                e_handle_msg_type msg_type,
                e_action_type act_type,
                void *arg)
{
    u16     src_port                = 0;                        ///< 源端口
    u32     log_str_len             = 0;                        ///< 定义日志字符串长度
    u32     encode_len              = 0;                        ///< base64编码后字符串长度
    char    src_ip[PLC_IP_LEN]      = {0};                      ///< 源IP
    char    log_str[LOG_SRT_LEN]    = {0};                      ///< 定义日志字符串
    char    encode_str[4096]        = {0};                      ///< base64编码后字符串
    char    *act_str[16]            = {"ADD", "DEL", "INQ"};    ///< 定义日志中记录动作的字符串


    t_action_add    *add_info       =
                        (t_action_add *)msg_info->data;         ///< 获取新增动作

    /* 获取远程服务器的IP和端口信息 */
    if(4 == conn->flag) ///< IPv4
    {
        inet_ntop(AF_INET, &conn->remote_addr.sin_addr, src_ip, PLC_IP_LEN);
        src_port = ntohs(conn->remote_addr.sin_port);
    }
    else    ///< IPv6
    {
        inet_ntop(AF_INET6, &conn->remote_addr_ipv6.sin6_addr, src_ip, PLC_IP_LEN);
        src_port = ntohs(conn->remote_addr_ipv6.sin6_port);
    }

    // 未开启日志开关
    if (!g_log_flag)
    {
        return;
    }

    log_str_len += snprintf(log_str + log_str_len, LOG_SRT_LEN - log_str_len,
                                "[%s]:%s, %s, %u, %u, %u",
                                act_str[act_type],
                                msg_info->date,
                                src_ip,
                                src_port,
                                add_info->user_id,
                                add_info->rule_id);

    if (action_add == act_type)
    {
        encode_len = comm_base64_encode((u8 *)msg_info, msg_info->data_len + sizeof(t_handle_msg_info), encode_str);
        if (!encode_len)
        {
            return;
        }

        log_str_len += snprintf(log_str + log_str_len, LOG_SRT_LEN - log_str_len, ",%.*s", encode_len, encode_str);
    }

    log_str_len += snprintf(log_str + log_str_len, LOG_SRT_LEN - log_str_len, "\n");
    PLCLOG(PRI_INFO, "%.*s", log_str_len, log_str);

    return;
}

