#include "libx.h"
#include "comm.h"

static int __msg_handle_connect_req(t_connection * conn,u8 * pos,u8 * tail,void * arg)
{
    int         i;
    t_user_info user_info;

    read_content(user_info.username, pos, tail, 16);
    read_content(user_info.passwd, pos, tail, 32);

    for(i=0; i<users_info.num; i++)
    {
        if(!strncmp(user_info.username, users_info.user_info[i].username, strlen(user_info.username)))
        {
            if(!strncmp(user_info.passwd, users_info.user_info[i].passwd, strlen(user_info.passwd)))
            {
                conn->user_id = users_info.user_info[i].user_id;
                break;
            }
            else
            {
                return -1;
            }
        }
    }

    if(i == users_info.num)
    {
        return -2;
    }
    return 0;
error:
    return -2;
}

static void pm_msg_connect_res(t_connection * conn, u16 error_num, u16 op_code)
{
    t_comm_msg *ack_msg;

    ack_msg = comm_msg_alloc(1024);
    if(!ack_msg)
    {
        return;
    }

    comm_msg_set_header(ack_msg, mh_msg_type, op_code);
    comm_msg_set_header(ack_msg, mh_msg_id, 0);

    comm_msg_append_u16(ack_msg, error_num, network_order);
    comm_msg_append_u16(ack_msg, op_code, network_order);
}


static int msg_handle_connect_req(t_connection * conn,t_comm_msg * msg,void * args)
{
    u32             data_len;
    u8              * pos;
    u8              * tail;
    int             ret;

    data_len = msg->msg_length;
    pos = msg->data;
    tail = msg->data + data_len;

    ret = __msg_handle_connect_req(conn,pos,tail,args);

    comm_msg_free(msg);
    return ret;
}

t_connection * comm_port_accept(t_tcp_port * port)
{
    t_connection    *conn;
    t_comm_msg      *msg;
    bool            ret     = false;

    conn = x_tcp_port_accept(port, sizeof(t_comm_connection_priv));
    if (conn) {
        comm_connection_priv_init(conn);
    }
    comm_msg_recv(conn, &msg);

    if(0x0101 != msg->msg_type)
    {
        pm_msg_connect_res(conn, PM_RET_INVALID_MSG_TYPE, 0x0102);
        close(conn->sockfd);
        return NULL;
    }

    ret = msg_handle_connect_req(conn, msg, NULL);
    if(-1 == ret)
    {
        printf("auth failed\n");
        pm_msg_connect_res(conn, PM_RET_INVALID_PASSWD, 0x0102);
        close(conn->sockfd);
        return NULL;
    }
    else if(-2 == ret)
    {
        printf("auth failed\n");
        pm_msg_connect_res(conn, PM_RET_INVALID_USER, 0x0102);
        close(conn->sockfd);
        return NULL;
    }

    printf("auth success\n");
    pm_msg_connect_res(conn, PM_RET_SUCCESS, 0x0102);

    return conn;

}

#if 0
t_connection * comm_port_accept(t_tcp_port * port)
{
    t_connection * conn;
    conn = x_tcp_port_accept(port, sizeof(t_comm_connection_priv));
    if (conn) {
        comm_connection_priv_init(conn);
    }
    return conn;
}
#endif
