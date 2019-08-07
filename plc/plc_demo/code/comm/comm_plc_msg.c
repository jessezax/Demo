#include "libx.h"
#include "comm.h"

#define MSG_VERSION             (0x01)

bool plc_msg_set_header(t_comm_plc_msg * msg,e_plc_msg_header header_idx,comm_u64 data)
{
    switch (header_idx)
    {
        case plc_msg_version:
            msg->version = data & 0xff;
            break;
        case plc_msg_code:
            msg->code = data & 0xff;
            break;
        case plc_msg_src_devid:
            msg->src_devid = data;
            break;
        case plc_msg_dst_devid:
            msg->dst_devid = data;
            break;
        case plc_msg_cookie:
            msg->cookie = data;
            break;
        default:
            return false;
    }
    return true;
}

static inline
comm_u32 * plc_msg_tail(t_comm_plc_msg * msg)
{
    comm_u32 * tail;
    comm_u32   len;
    len = ntohs(msg->length);

    tail = (comm_u32 *)(msg->data + len - sizeof(t_comm_plc_msg));
    return tail;
}

static inline
comm_u32 plc_msg_read_tail_room(t_comm_plc_msg * msg)
{
    comm_u32 * tail = plc_msg_tail(msg);
    return *tail;
}

static inline
void plc_msg_set_tail_room(t_comm_plc_msg * msg,comm_u32 size)
{
    comm_u32 * tail =plc_msg_tail(msg);
    *tail = size;
}

static inline
void plc_msg_length_add(t_comm_plc_msg * msg,comm_u32 size)
{
    msg->length = htons(ntohs(msg->length) + size);
}

bool plc_msg_append(t_comm_plc_msg * msg,void * data,comm_u32 size)
{
    comm_u32 * tail      = plc_msg_tail(msg);
    comm_u32   tail_room = plc_msg_read_tail_room(msg);

    if (tail_room < size)
    {
        return false;
    }

    memcpy(tail,data,size);
    plc_msg_length_add(msg,size);
    plc_msg_set_tail_room(msg,tail_room - size);
    return true;
}

bool plc_msg_append_byte(t_comm_plc_msg * msg,comm_u8 val)
{
    return plc_msg_append(msg,&val,sizeof(val));
}

bool plc_msg_append_word(t_comm_plc_msg * msg,comm_u16 val,comm_u8 byte_order)
{
    if (byte_order == network_order) {
        val = htons(val);
    }
    return plc_msg_append(msg,&val,sizeof(val));
}

bool plc_msg_append_dword(t_comm_plc_msg * msg,comm_u32 val,comm_u8 byte_order)
{
    if (byte_order == network_order)
    {
        val = htonl(val);
    }
    return plc_msg_append(msg,&val,sizeof(val));
}

bool plc_msg_append_string(t_comm_plc_msg * msg,char * val,comm_u32 size,comm_u8 len_size)
{
    if (len_size == sizeof(comm_u8))
    {
        if (!plc_msg_append_byte(msg,size))
        {
            return false;
        }
    }
    else if (len_size == sizeof(comm_u16))
    {
        if (!plc_msg_append_word(msg,size,network_order))
        {
            return false;
        }
    }
    else if (len_size == sizeof(comm_u32))
    {
        if (!plc_msg_append_dword(msg,size,network_order))
        {
            return false;
        }
    }
    else
    {
        return false;
    }
    return plc_msg_append(msg,val,size);
}

t_comm_plc_msg * plc_msg_alloc(void)
{
    t_comm_plc_msg * msg;
    msg = comm_zalloc(sizeof(t_comm_plc_msg) + comm_PLC_MSG_SIEZ + sizeof(comm_u32));
    if (!msg)
    {
        return NULL;
    }
    plc_msg_set_header(msg,plc_msg_version,MSG_VERSION);
    plc_msg_length_add(msg,sizeof(t_comm_plc_msg));
    plc_msg_set_tail_room(msg,comm_PLC_MSG_SIEZ);
    return msg;
}

void plc_msg_free(t_comm_plc_msg * msg)
{
    comm_free(msg);
}
