#include "libx.h"
#include "comm.h"

bool read_account_info(t_account_info *dst, u8 ** pos,u8 ** tail)
{
    u8  cmb_flag;                                           ///< 账号类型4bit+保留位4bit

    read_u8(cmb_flag, *pos, *tail);                         ///< 读取组合位，4bit账号+4bit保留
    dst->account_type = (cmb_flag >> 4) & 0x0f;             ///< 获取账号 4bit
    read_u8(cmb_flag, *pos, *tail);                         ///< 跳过1byte保留位
    read_content(dst->account, *pos, *tail, 64);            ///< 读取账号信息
    read_content(dst->account_mask, *pos, *tail, 64);       ///< 读取账号掩码信息

    if (dst->account_type > 3)
    {
        goto error;
    }

    return legality_check_for_str(dst->account, dst->account_mask, PLC_ACCOUNT_LEN);
error:
    return false;
}

t_handle_msg_info *account_add(u8 * pos,u8 * tail, t_connection * conn, u32 *__rule_id, e_comm_return_code *error_num, u8 user_id, void *arg)
{
    u8                  cmb_tag;                            ///< 动作、重启保存、中标统计组合标签
    int                 add_len                 = 0;
    int                 acc_len                 = 0;
    t_action_add        add_info;                           ///< 增加操作
    t_account_info      acc_info;                           ///< 账号信息
    t_handle_msg_info   *handle_msg             = NULL;

    char            data[1024]                  = {0};      ///< 用于构建handle_msg的data部分
    char            start_time[32]              = {0};
    char            ip_addr[64]                 = {0};      ///< 用于记录策略服务器IP
    u16             port;

    memset(&add_info, 0x0, sizeof(t_action_add));
    memset(&acc_info, 0x0, sizeof(t_account_info));

    read_u8(add_info.user_id, pos, tail);                   ///< 读取用户ID
    read_u8(add_info.port_group, pos, tail);                ///< 读取输出端口组号
    read_u8(cmb_tag, pos, tail);                            ///< 读取动作、重启保存、中标统计组合标签
    read_u8(add_info.isp, pos, tail);                       ///< 读取运营商信息
    read_u32(add_info.rule_id, pos, tail, host_order);      ///< 读取规则ID

    *__rule_id = add_info.user_id;
    if (add_info.user_id != user_id)
    {
        goto user_error;
    }

    /* 判断在现有的链表中是否可以找到 */
    if (pm_plc_data_search(add_info.user_id, add_info.rule_id))
    {
        goto exist_error;
    }

    read_u32(add_info.timestamp, pos, tail, host_order);    ///< 读取时间戳信息
    read_u8(add_info.type[0], pos, tail);                   ///< 读取条件一规则类型
    read_u8(add_info.type[1], pos, tail);                   ///< 读取条件二规则类型
    read_u8(add_info.type[2], pos, tail);                   ///< 读取条件三规则类型
    read_u8(add_info.type[3], pos, tail);                   ///< 读取保留位

    add_info.act    = (cmb_tag >> 6) & 0x03;                ///< 获取动作 2bit
    add_info.save   = (cmb_tag >> 5) & 0x01;                ///< 获取重启保存 1bit
    add_info.hst    = (cmb_tag >> 4) & 0x01;                ///< 获取中标统计 1bit
    time2str(add_info.timestamp, start_time, 32);           ///< 将时间戳转换为 2019-01-01 02:03:04

    /* 判断是否是类型是否是账号 */
    if (comm_sub_msg_account != (add_info.type[0] & 0x7f))
    {
        goto type_error;
    }

    /* 读取账号信息 */
    if (!read_account_info(&acc_info, &pos, &tail))
    {
        goto error;
    }

    /* 获取远程服务器的IP和端口信息 */
    if(4 == conn->flag) ///< IPv4
    {
        inet_ntop(AF_INET, &conn->remote_addr.sin_addr, ip_addr, 64);
        port = ntohs(conn->remote_addr.sin_port);
    }
    else    ///< IPv6
    {
        inet_ntop(AF_INET6, &conn->remote_addr_ipv6.sin6_addr, ip_addr, 64);
        port = ntohs(conn->remote_addr_ipv6.sin6_port);
    }

    add_len = sizeof(t_action_add);                         ///< 新增消息的长度
    acc_len = sizeof(t_account_info);                       ///< 账号信息的长度

    ///< 组合生成hanle_msg的data部分
    memcpy(data, &add_info, add_len);
    memcpy(data + add_len, &acc_info, acc_len);

    /* 生成账号信息，并将其添加到账号信息的未入库链表中 */
    handle_msg = handle_msg_info_alloc(comm_sub_msg_account, ip_addr, port, add_len + acc_len, data);
    if (!handle_msg)
    {
        goto error;
    }

    if (!pm_plc_data_insert(add_info.user_id, add_info.rule_id, handle_msg, mysql_out_list))
    {
        goto error;
    }

    *error_num  = PM_RET_SUCCESS;
    return handle_msg;

user_error:
    *error_num  = PM_RET_INVALID_USER;
    return NULL;
type_error:
    *error_num  = PM_RET_INVALID_MSG_TYPE;
    return NULL;
exist_error:
    *error_num  = PM_RET_RULE_EXIST;
    return NULL;
error:
    *error_num = PM_RET_UNKNOWN_ERROR;
    return NULL;
}

static bool comm_msg_handle_account(t_connection * conn,u16 msg_id,e_comm_msg_action_type msg_action,u8 * pos,u8 * tail,void * arg)
{
    t_handle_msg_info   *handle_msg         = NULL;

    e_comm_return_code  error_num           = 0;
    u32                 rule_id             = 0;

    if (comm_msg_action_add == msg_action)  ///< 新增信息
    {
        handle_msg = account_add(pos, tail, conn, &rule_id, &error_num, conn->user_id, arg);
        if (!handle_msg)
        {
            goto failed;
        }
        else
        {
            curr_increase_rule_num++;
            curr_full_rule_num++;
            plc_log(conn,handle_msg, handle_msg_account, action_add, NULL);
            goto add_success;
        }
    }
    else if (comm_msg_action_del == msg_action) ///< 删除信息
    {
        handle_msg = load_info_for_del(conn, pos, tail, handle_msg_account, &rule_id, &error_num, arg);
        if (!handle_msg)
        {
            goto failed;
        }
        else
        {
            curr_increase_rule_num++;
            curr_full_rule_num++;
            plc_log(conn, handle_msg, handle_msg_account, action_del, NULL);
            goto del_success;
        }
    }
    else if (comm_msg_action_inq == msg_action) ///< 查询信息
    {
        handle_msg = load_info_for_inq(conn, pos, tail, handle_msg_account, &rule_id, &error_num, arg);
        if (!handle_msg)
        {
            goto failed;
        }
        else
        {
            plc_log(conn, handle_msg, handle_msg_account, action_inq, NULL);
            goto inq_success;
        }
    }
    else
    {
        error_num = PM_RET_INVALID_MSG_TYPE;
        goto failed;
    }

    return true;

/* 消息错误或者失败ACK回复 */
failed:
    pm_msg_ack_send(conn, rule_id, msg_action, comm_msg_account_req, error_num, NULL);
    return false;
/* 新增消息成功ACK回复 */
add_success:
    rule_generate_for_inc(conn, comm_msg_account_req, action_add, handle_msg, rule_file_type_comm, NULL);
    g_rule_count++;
    if(g_rule_count == g_max_plc_num_for_ack)
    {
        pm_msg_ack_send(conn, rule_id, msg_action, comm_msg_account_req, error_num, NULL);
        g_rule_count = 0;
    }
    return true;
/* 删除消息成功ACK回复 */
del_success:
    rule_generate_for_inc(conn, comm_msg_account_req, action_del, handle_msg, rule_file_type_comm, NULL);
    pm_msg_ack_send(conn, rule_id, msg_action, comm_msg_account_req, error_num, NULL);
    return true;
/* 查询消息成功ACK回复 */
inq_success:
    pm_msg_ack_send(conn, rule_id, msg_action, comm_msg_account_req, error_num, &handle_msg->hit_counts);
    return true;
}

static int comm_account_init(t_xmodule * xmod)
{
    comm_msg_handler_register("account", comm_msg_account_req, comm_msg_handle_account);
    return 0;
}

static int comm_account_uninit(t_xmodule * xmod)
{
    return 0;
}

static int comm_account_reconf(t_xmodule * xmod)
{
    return 0;
}

static int comm_account_ctrl(t_xmodule * xmod,unsigned int ctrl,void * arg,unsigned int arg_size)
{
    return 0;
}

static t_xmodule comm_xmod_account = {
    .name   = "account",
    .idx    = XMOD_ACCOUNT_IDX,
    .init   = comm_account_init,
    .uninit = comm_account_uninit,
    .reconf = comm_account_reconf,
    .ctrl   = comm_account_ctrl,
    .ctx    = NULL
};

x_module_register(&comm_xmod_account, XMOD_ACCOUNT_IDX)

