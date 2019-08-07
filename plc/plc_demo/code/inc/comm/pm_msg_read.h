#ifndef __PM_MSG_READ_H__
#define __PM_MSG_READ_H__

bool read_ip_port_pro_info(e_comm_msg_sub_type ip_type, t_ip_port_pro_info *dst, u8 ** pos,u8 ** tail);
bool read_condi_code_info(t_cond_code_info *dst, u8 **pos, u8 **tail);
bool read_account_info(t_account_info *dst, u8 **pos, u8 **tail);
bool read_pro_app_info(t_pro_app_info *dst, u8 **pos, u8 **tail);
bool read_audio_video_info(t_audio_video_info *dst, u8 **pos, u8 **tail);
bool read_http_info(t_http_info *dst, u8 **pos, u8 **tail);
bool read_dns_info(t_dns_info *dst, u8 **pos, u8 **tail);
bool read_model_info(t_model_info *dst, u8 **pos, u8 **tail);

bool rule_id_flag_init();

#endif
