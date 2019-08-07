#include "libx.h"
#include "comm.h"

static f_comm_plc_msg_handle g_comm_plc_msg_handles[256] = {NULL};

bool comm_plc_msg_register(e_comm_plc_type type,f_comm_plc_msg_handle handle)
{
    if (g_comm_plc_msg_handles[type]) {
        return false;
    }
    g_comm_plc_msg_handles[type] = handle;
    return true;
}

void comm_plc_msg_handle(t_comm_plc_port * plc_port,t_comm_plc_msg * msg,struct sockaddr_in * remote_addr,void* arg)
{
    if (g_comm_plc_msg_handles[msg->code]) {
        g_comm_plc_msg_handles[msg->code](plc_port,msg,remote_addr,arg);
    }
    plc_msg_free(msg);
}

