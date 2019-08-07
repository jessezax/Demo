#include "libx.h"
#include "comm.h"

typedef struct {
    t_string            name;
    u16                 msg_type;
    f_comm_msg_handle   handle;
}t_comm_msg_handler;

static t_comm_msg_handler * g_comm_msg_handler[256] = {NULL};

bool comm_msg_handler_register(const char * name,u8 msg_type,f_comm_msg_handle handle)
{
    t_comm_msg_handler * h;
    if (g_comm_msg_handler[msg_type]) {
        return false;
    }
    h = x_malloc(sizeof(t_comm_msg_handler));
    if (!h) {
        return false;
    }
    if (!__string_dup(name,strlen(name),&h->name)) {
        x_free(h);
        return false;
    }

    h->handle = handle;
    h->msg_type = msg_type;

    g_comm_msg_handler[msg_type] = h;
    return true;
}

void __comm_msg_handle(t_connection * conn,u8 msg_type,e_comm_msg_action_type msg_action,u16 msg_id,u8 * pos,u8 * tail,void * args)
{

    t_comm_msg_handler * h;
    h = g_comm_msg_handler[msg_type];
    if (h) {
        if (h->handle) {
            h->handle(conn,msg_id,msg_action,pos,tail,args);
        }
    }
}

void comm_msg_handle(t_connection * conn,t_comm_msg * msg,void * args)
{
    u32     data_len;
    u8      * pos;
    u8      * tail;

    data_len = msg->msg_length;
    pos = msg->data;
    tail = msg->data + data_len;

    #if 0
    t_action_add *add_info = (t_action_add *)msg->data;
    int i;
    for (i=0; i<2000000; i++)
    {
        add_info->rule_id++;
        __comm_msg_handle(conn,msg->msg_type,msg->msg_type>>8,msg->msg_id,pos,tail,args);
    }
    #endif
    __comm_msg_handle(conn,msg->msg_type,msg->msg_type>>8,msg->msg_id,pos,tail,args);
    comm_msg_free(msg);
    return;
}

bool comm_msg_set_header(t_comm_msg * msg,e_comm_header header_idx,u32 data)
{
    switch (header_idx) {
        case mh_msg_type:
            msg->msg_type = ntohs(data) & 0xffff;
            break;
        case mh_msg_id:
            msg->msg_id = ntohs(0x0111);
            break;
        default:
            return false;
    }
    return true;
}

static inline
u32 * comm_msg_tail(t_comm_msg * msg)
{
    u32 *tail;
    u32 len;
    len = ntohl(msg->msg_length);

    tail = (u32 *)(msg->data + len - sizeof(t_comm_msg));
    return tail;
}

static inline
u32 comm_msg_read_tail_room(t_comm_msg * msg)
{
    u32 * tail = comm_msg_tail(msg);
    return *tail;
}

static inline
void comm_msg_set_tail_room(t_comm_msg * msg,u32 size)
{
    u32 * tail = comm_msg_tail(msg);
    *tail = size;
}

static inline
void comm_msg_length_add(t_comm_msg * msg,u32 size)
{
    msg->msg_length = htonl(ntohl(msg->msg_length) + size);
}

bool comm_msg_append(t_comm_msg * msg,void * data,u32 size)
{
    u32 * tail      = comm_msg_tail(msg);
    u32   tail_room = comm_msg_read_tail_room(msg);

    if (tail_room < size) {
        return false;
    }

    memcpy(tail,data,size);
    comm_msg_length_add(msg,size);
    comm_msg_set_tail_room(msg,tail_room - size);
    return true;
}

bool comm_msg_append_u8(t_comm_msg * msg,u8 val)
{
    return comm_msg_append(msg,&val,sizeof(val));
}

bool comm_msg_append_u16(t_comm_msg * msg,u16 val,u8 byte_order)
{
    if (byte_order == network_order) {
        val = htons(val);
    }
    return comm_msg_append(msg,&val,sizeof(val));
}

bool comm_msg_append_u32(t_comm_msg * msg,u32 val,u8 byte_order)
{
    if (byte_order == network_order) {
        val = htonl(val);
    }
    return comm_msg_append(msg,&val,sizeof(val));
}

#if 1
bool comm_msg_append_u64(t_comm_msg * msg,u64 val,u8 byte_order)
{
    if (byte_order == network_order) {
        val = htonll(val);
    }
    return comm_msg_append(msg,&val,sizeof(val));
}
#endif

bool __comm_msg_append_string(t_comm_msg * msg,char * val,u32 size)
{
    if (!comm_msg_append_u8(msg,size)) {
        return size;
    }
    return comm_msg_append(msg,val,size);
}

bool comm_msg_append_string(t_comm_msg * msg,t_string * string)
{
    return __comm_msg_append_string(msg,string->data,string->len);
}

bool __comm_msg_append_string2(t_comm_msg * msg,char * val,u32 size)
{
    if (!comm_msg_append_u16(msg,size,network_order)) {
        return size;
    }
    return comm_msg_append(msg,val,size);
}

bool comm_msg_append_string2(t_comm_msg * msg,t_string * string)
{
    return __comm_msg_append_string2(msg,string->data,string->len);
}

t_comm_msg * comm_msg_alloc(u32 append_max)
{
    t_comm_msg * msg;

    msg = x_zalloc(sizeof(t_comm_msg) + append_max + sizeof(u32));
    if (!msg) {
        return NULL;
    }
    //comm_msg_set_header(msg,mh_msg_version,0x00);
    comm_msg_length_add(msg,sizeof(t_comm_msg));
    comm_msg_set_tail_room(msg,append_max);
    return msg;
}

void comm_msg_free(t_comm_msg * msg)
{
    x_free(msg);
}

bool comm_msg_recv(t_connection * conn,t_comm_msg ** __msg)
{
    t_comm_msg   msg_header;
    t_comm_msg * msg;
    ssize_t      data_len;
    ssize_t      recv_len;

    recv_len = x_connection_recv(conn, &msg_header, sizeof(t_comm_msg));
    if (recv_len != sizeof(t_comm_msg)) {
        return false;
    }

    msg_header.msg_id       = ntohs(msg_header.msg_id);
    msg_header.msg_type     = ntohs(msg_header.msg_type);
    msg_header.msg_length   = ntohl(msg_header.msg_length);
    data_len                = msg_header.msg_length;
    msg                     = comm_msg_alloc(data_len);
    if (!msg) {
        return false;
    }
    memcpy(msg,&msg_header,sizeof(t_comm_msg));
    recv_len = x_connection_recv(conn, msg->data, data_len);
    if (recv_len != data_len) {
        comm_msg_free(msg);
        return false;
    }
    *__msg = msg;
    conn->msg_len = msg->msg_length;
    return true;
}

bool comm_msg_send(t_connection * conn,t_comm_msg * msg)
{
    ssize_t  send_len;
    ssize_t  buf_len;
    bool     retval = true;

    buf_len = ntohl(msg->msg_length);
    msg->msg_length = htonl(ntohl(msg->msg_length) - sizeof(t_comm_msg));

    send_len = x_connection_send(conn, msg, buf_len);
    if (send_len != buf_len) {
        retval = false;
    }
    comm_msg_free(msg);
    return retval;
}

