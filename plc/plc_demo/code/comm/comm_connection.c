#include "libx.h"
#include "comm.h"

void comm_set_mysql_freq(t_connection * conn,u32 sec)
{
    t_comm_connection_priv * priv = comm_connection_priv(conn);
    priv->mysql_freq = sec;
    priv->last_mysql = clock_monotonic();
}

void comm_connection_priv_init(t_connection * conn)
{
    t_comm_connection_priv * priv;
    priv = comm_connection_priv(conn);
    priv->connection = conn;
    priv->msg_idx = rand();
    comm_set_mysql_freq(conn,DEFAULT_MYSQL_FREQ);
    conn->last_time = clock_monotonic();
}

///< 发送认证信息
void pm_authentication_info_send(t_connection * conn)
{
    t_comm_msg      *auth_msg;
    unsigned char   passwd_md5[128] = {0};

    MD5_CTX ctx;
    MD5_Init(&ctx);
    MD5_Update(&ctx, g_passwd, strlen(g_passwd));
    MD5_Final(passwd_md5, &ctx);

    auth_msg = comm_msg_alloc(1024);
    if(!auth_msg)
    {
        return;
    }
    comm_msg_set_header(auth_msg, mh_msg_type, 0x0101);
    comm_msg_set_header(auth_msg, mh_msg_id, 0x0111);

    comm_msg_append(auth_msg, g_user_name, 16);
    comm_msg_append(auth_msg, passwd_md5, 32);

    comm_msg_send(conn, auth_msg);

    return;
}

static bool pm_auth_ret_msg_handle(t_connection *conn, t_comm_msg *msg, void *args)
{
    u32     data_len;
    u8      *pos;
    u8      *tail;
    u16     msg_type;
    u16     ret_code;

    data_len = msg->msg_length - sizeof(t_comm_msg);
    pos = msg->data;
    tail = msg->data + data_len;
    if(msg->msg_type != 0x0102)
    {
        return false;
    }

    read_u16(msg_type, pos, tail, host_order);
    read_u16(ret_code, pos, tail, host_order);
    if((msg_type != 0x0101) || (ret_code != 0x0000))
    {
        return false;
    }
    return true;
error:
    return false;
}

bool authentication(t_connection *conn)
{
    int         reconnect_cnt = 0;
    t_comm_msg  *msg;

    while(!x_connection_connect(conn) && reconnect_cnt < 3) {
        reconnect_cnt++;
    }
    if(3 == reconnect_cnt)
    {
        goto connect_error;
    }


    pm_authentication_info_send(conn);

    if(comm_msg_recv(conn, &msg))
    {
        if(!pm_auth_ret_msg_handle(conn, msg, NULL))
        {
            goto auth_error;
        }
    }
    else
    {
        goto auth_error;
    }

    return true;
auth_error:
    printf("authenticate failed!\n");
    return false;
connect_error:
    printf("connect authenticate server failed!\n");
    return false;
}

t_connection * comm_connection_alloc(struct sockaddr_in * remote_addr)
{
    t_connection * conn;

    conn = x_connection_alloc(remote_addr,sizeof(t_comm_connection_priv));
    if (conn) {
        comm_connection_priv_init(conn);
    }

    if(!authentication(conn))
    {
        return NULL;
    }

    return conn;
}

t_connection * comm_ipv6_connection_alloc(struct sockaddr_in6 * remote_addr)
{
    t_connection * conn;

    conn = x_connection_ipv6_alloc(remote_addr, sizeof(t_comm_connection_priv));
    if(conn){
        comm_connection_priv_init(conn);
    }

    if(!authentication(conn))
    {
        return NULL;
    }

    return conn;
}


t_connection * comm_connection_alloc_by_accecpt(int sockfd,struct sockaddr_in * remote_addr)
{
    t_connection * conn;

    conn = x_connection_alloc_by_accecpt(sockfd,remote_addr,sizeof(t_comm_connection_priv));
    if (conn) {
        comm_connection_priv_init(conn);
    }
    return conn;
}

t_connection * comm_ipv6_connection_alloc_by_accecpt(int sockfd,struct sockaddr_in6 * remote_addr)
{
    t_connection * conn;

    conn = x_connection_ipv6_alloc_by_accecpt(sockfd,remote_addr,sizeof(t_comm_connection_priv));
    if (conn) {
        comm_connection_priv_init(conn);
    }
    return conn;
}
