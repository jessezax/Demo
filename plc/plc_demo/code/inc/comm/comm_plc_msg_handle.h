#ifndef __comm_PLC_NOTIFY_MSG_H__
#define __comm_PLC_NOTIFY_MSG_H__

typedef enum {
    comm_plc_discover    = 0x00,
    comm_plc_offer       = 0x01,
    comm_plc_heartbeat   = 0x02,
    comm_plc_syn         = 0x10,
    comm_plc_userip      = 0x11,
    comm_plc_usergroup,
    comm_plc_custom_proto_dat,
    comm_plc_custom_http_proto,
    comm_plc_custom_proto_list,
    comm_plc_xdr_temp,
    comm_plc_pcap,
    comm_plc_mirror,
    comm_plc_ctrl,
    comm_plc_filter,
    comm_plc_xs_comm,
    comm_plc_xs_web,
    comm_plc_xs_tra,
    comm_plc_xs_session,
    lte_full_update,                ///< M项目全量更新
    lte_incremental_update,         ///< M项目增量更新
    lte_tuple_full_update,          ///< M项目五元组全量更新
    lte_tuple_incremental_update,   ///< M项目五元组增量更新
    lte_cmb_full_update,            ///< M项目单匹配项全量更新
    lte_cmb_incremental_update, ///< M项目单匹配项增量更新
    lte_flow_full_update,           ///< M项目flow规则全量更新
    lte_flow_incremental_update,    ///< M项目flow规则增量更新
    comm_plc_max
}e_comm_plc_type;


typedef enum {
    comm_data_type_data,
    comm_data_type_http_uri,
    comm_data_type_http_url,
    comm_data_type_ftp_uri,
    comm_data_type_ftp_url
}e_comm_data_type;

typedef struct {
    comm_u32  version;
    comm_u8   type;
    char      data[0];
} __attribute__((__packed__)) t_comm_plc_data;

typedef void (*f_comm_plc_msg_handle)(t_comm_plc_port *,t_comm_plc_msg *,struct sockaddr_in *,void*);
bool comm_plc_msg_register(e_comm_plc_type type,f_comm_plc_msg_handle handle);
void comm_plc_msg_handle(t_comm_plc_port * plc_port,t_comm_plc_msg * msg,struct sockaddr_in * remote_addr,void* arg);

#endif
