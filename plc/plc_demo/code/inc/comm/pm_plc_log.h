#ifndef __PM_PLC_LOG_H__
#define __PM_PLC_LOG_H__

extern rsn_log_ctx *g_plc_log_ctx;

/*! 记录到默认日志文件 */
#define PLCLOG(pri, fmt, ...) rsn_log(g_plc_log_ctx, pri, fmt, ##__VA_ARGS__);

void plc_log(   t_connection * conn,
                t_handle_msg_info *msg_info,
                e_handle_msg_type msg_type,
                e_action_type act_type,
                void *arg);

#endif
