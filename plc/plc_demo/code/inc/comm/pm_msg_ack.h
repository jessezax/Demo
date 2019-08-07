#ifndef __PM_MSG_ACK_H__
#define __PM_MSG_ACK_H__

bool pm_failed_msg_ack_send(t_connection * conn, u32 rule_id, u16 error_num, u16 op_code);
bool pm_msg_ack_send(   t_connection * conn,
                        u32 rule_id,
                        e_comm_msg_action_type act,
                        e_comm_msg_type type,
                        e_comm_return_code ret_code,
                        void *arg);

#endif
