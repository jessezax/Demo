#ifndef __comm_PLC_MSG_H__
#define __comm_PLC_MSG_H__

#define BROADCAST_DEVID (0xFFFFFFFFFFFFFFFF)

#define comm_PLC_MSG_SIEZ       (2048)

typedef enum {
    plc_msg_version,
    plc_msg_code,
    plc_msg_length,
    plc_msg_src_devid,//发送时由dn_port_send函数设置
    plc_msg_dst_devid,
    plc_msg_cookie
}e_plc_msg_header;

#if 0
enum {
    network_order = 0,
    host_order    = 1
};
#endif

typedef struct
{
    comm_u8        version;
    comm_u8        code;
    comm_u16       length;
    comm_u64       cookie;
    comm_u64       src_devid;
    comm_u64       dst_devid;
    comm_u8        data[0];
} __attribute__((__packed__)) t_comm_plc_msg;///<policy message header


t_comm_plc_msg * plc_msg_alloc(void);
void plc_msg_free(t_comm_plc_msg * msg);

bool plc_msg_append_string(t_comm_plc_msg * msg,char * val,comm_u32 size,comm_u8 len_size);
bool plc_msg_append_dword(t_comm_plc_msg * msg,comm_u32 val,comm_u8 byte_order);
bool plc_msg_append_word(t_comm_plc_msg * msg,comm_u16 val,comm_u8 byte_order);
bool plc_msg_append_byte(t_comm_plc_msg * msg,comm_u8 val);
bool plc_msg_append(t_comm_plc_msg * msg,void * data,comm_u32 size);
bool plc_msg_set_header(t_comm_plc_msg * msg,e_plc_msg_header header_idx,comm_u64 data);

#endif
