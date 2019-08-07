#include "libx.h"
#include "comm.h"
#if 0
bool pm_failed_msg_ack_send(t_connection * conn, u32 rule_id, u16 error_num, u16 op_code)
{
    t_comm_msg              *ack_msg        = NULL;

    ack_msg = comm_msg_alloc(2048);
    if (!ack_msg) {
        return false;
    }

    comm_msg_set_header(ack_msg, mh_msg_type, op_code);
    comm_msg_set_header(ack_msg, mh_msg_id, 0);

    comm_msg_append_u16(ack_msg, error_num, network_order);
    comm_msg_append_u16(ack_msg, op_code, network_order);
    comm_msg_append_u32(ack_msg, rule_id, network_order);

    return comm_msg_send(conn, ack_msg);
}
#endif

/**
 * @brief                   策略新增消息回复
 * @param[in]  conn         连接
 * @param[in]  rule_id      策略ID
 * @param[in]  act          策略动作
 * @param[in]  type         策略消息错误状态
 * @param[in]  ret_code     策略消息返回码
 * @param[in]  arg          策略相应消息扩展字段
 * @return                  bool
 */
bool pm_msg_ack_send(   t_connection * conn,
                        u32 rule_id,
                        e_comm_msg_action_type act,
                        e_comm_msg_type type,
                        e_comm_return_code ret_code,
                        void *arg)
{
    u16         op_code     = 0;
    t_comm_msg  *ack_msg    = NULL;
    u64         hit_count   = 0;

    ack_msg = comm_msg_alloc(2048);
    if (!ack_msg) {
        return false;
    }

    op_code = act;
    op_code = ((op_code << 8) & 0xff00) + type;

    if (comm_msg_http_req == type)
    {
        comm_msg_set_header(ack_msg, mh_msg_type, op_code + 2);
    }
    else
    {
        comm_msg_set_header(ack_msg, mh_msg_type, op_code + 1);
    }
    comm_msg_set_header(ack_msg, mh_msg_id, 0);

    comm_msg_append_u16(ack_msg, op_code,   network_order);
    comm_msg_append_u16(ack_msg, ret_code,  network_order);
    comm_msg_append_u32(ack_msg, rule_id,   network_order);

    if(arg)
    {
        hit_count = *(u64 *)arg;
        comm_msg_append_u64(ack_msg, hit_count, network_order);
    }

    return comm_msg_send(conn, ack_msg);
}

