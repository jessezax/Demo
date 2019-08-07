#include "libx.h"

t_connection * x_connection_alloc(struct sockaddr_in * remote_addr,u32 priv_size)
{
    t_connection * conn;

    conn = x_zalloc(sizeof(t_connection) + priv_size);
    if (!conn) {
        return NULL;
    }
    mutex_init(&conn->mutex);
    conn->remote_addr = *remote_addr;
    conn->flag        = 4;
    return conn;
}

t_connection * x_connection_ipv6_alloc(struct sockaddr_in6 * remote_addr,u32 priv_size)
{
    t_connection * conn;

    conn = x_zalloc(sizeof(t_connection) + priv_size);
    if (!conn) {
        return NULL;
    }
    mutex_init(&conn->mutex);
    conn->remote_addr_ipv6 = *remote_addr;
    conn->flag = 6;
    return conn;
}

t_connection * x_connection_alloc_by_accecpt(int sockfd,struct sockaddr_in * remote_addr,u32 priv_size)
{
    t_connection * conn;
    conn = x_connection_alloc(remote_addr,priv_size);
    if (!conn) {
        return NULL;
    }
    conn->sockfd = sockfd;
    conn->by_accept = true;
    conn->connected = true;
    return conn;
}

t_connection * x_connection_ipv6_alloc_by_accecpt(int sockfd,struct sockaddr_in6 * remote_addr,u32 priv_size)
{
    t_connection * conn;
    conn = x_connection_ipv6_alloc(remote_addr,priv_size);
    if (!conn) {
        return NULL;
    }
    conn->sockfd = sockfd;
    conn->by_accept = true;
    conn->connected = true;
    return conn;
}


bool x_connection_connect(t_connection * conn)
{
    socklen_t addrlen = 0;
    int sockfd = -1;
    if (conn->by_accept || conn->connected) {
        return false;
    }
    if(4 == conn->flag){
        addrlen= sizeof(struct sockaddr_in);
        sockfd = create_tcp_socket();
    }else if(6 == conn->flag){
        addrlen= sizeof(struct sockaddr_in6);
        sockfd = create_tcp_socket_ipv6();
    }
    if (sockfd < 0) {
        return false;
    }
    if (m_connect(sockfd, (struct sockaddr *)&conn->remote_addr, addrlen) < 0) {
        goto connect_error;
    }
    conn->sockfd = sockfd;
    conn->connected = true;
    return true;
connect_error:
    close(sockfd);
    return false;
}

void x_connection_colse(t_connection * conn)
{
    if (!conn->connected) {
        return;
    }
    conn->connected = false;
    conn->error = false;
    conn->recv_error = false;
    conn->send_error = false;
    close(conn->sockfd);
    conn->sockfd = -1;
}

void x_connection_free(t_connection * conn)
{
    x_connection_colse(conn);
    x_free(conn);
}

ssize_t x_connection_recv(t_connection * conn,void * buf,size_t size)
{
    ssize_t retval = -1;
    if (conn->connected && !conn->error) {
        retval = recv_all_size(conn->sockfd, buf, size);
        if (retval < 0) {
            mutex_lock(&conn->mutex);
            conn->error = true;
            conn->recv_error = true;
            mutex_unlock(&conn->mutex);
        }
    }
    return retval;
}

ssize_t x_connection_send(t_connection * conn,void * buf,size_t size)
{
    ssize_t retval = -1;

    if (conn->connected && !conn->error) {
        mutex_lock(&conn->mutex);
        retval = send_all_size(conn->sockfd, buf, size);
        if (retval < 0) {
            //conn->error = true;
            //conn->send_error = true;
        }
        mutex_unlock(&conn->mutex);
    }
    return retval;
}

