#include "libx.h"

t_tcp_port * x_tcp_port_alloc(struct sockaddr_in * local_addr)
{
    t_tcp_port * port;
    socklen_t addrlen = sizeof(struct sockaddr_in);
    int reuse = 1;

    port = x_zalloc(sizeof(t_tcp_port));
    if (!port) {
        return NULL;
    }

    port->sockfd = create_tcp_socket();
    if (port->sockfd < 0) {
        goto create_tcp_socket_error;
    }
    port->flag = 4;
    if (setsockopt(port->sockfd, SOL_SOCKET, SO_REUSEADDR,(const void *)&reuse , sizeof(int)) < 0) {
        goto setsockopt_error;
    }

    if (bind(port->sockfd, (struct sockaddr *)local_addr, addrlen) < 0) {
        goto bind_error;
    }

    if (listen(port->sockfd, 1024) < 0) {
        goto listen_error;
    }

    return port;
listen_error:
bind_error:
setsockopt_error:
    close(port->sockfd);
    port->sockfd = -1;
create_tcp_socket_error:
    x_free(port);
    return NULL;
}

t_tcp_port * x_tcp_port_ipv6_alloc(struct sockaddr_in6 * local_addr)
{
    t_tcp_port *port;
    socklen_t addr_len = sizeof(struct sockaddr_in6);;
    int reuse = 1;

    port = x_zalloc(sizeof(t_tcp_port));
    if(!port){
        return NULL;
    }

    port->sockfd = create_tcp_socket_ipv6();
    if(port->sockfd < 0){
        goto create_tcp_socket_ipv6_error;
    }
    port->flag = 6;
    if (setsockopt(port->sockfd, SOL_SOCKET, SO_REUSEADDR,(const void *)&reuse , sizeof(int)) < 0) {
        goto setsockopt_error;
    }

    if(bind(port->sockfd, (struct sockaddr *)local_addr, addr_len) < 0){
        goto bind_error;
    }
    if (listen(port->sockfd, 1024) < 0) {
        goto listen_error;
    }
    return port;
listen_error:
bind_error:
setsockopt_error:
    close(port->sockfd);
    port->sockfd = -1;
create_tcp_socket_ipv6_error:
    x_free(port);
    return NULL;
}

void x_tcp_port_free(t_tcp_port * port)
{
    if (port->sockfd > 0) {
        close(port->sockfd);
        port->sockfd = -1;
    }
    x_free(port);
}

t_connection * x_tcp_port_accept(t_tcp_port * port,u32 priv_size)
{
    int sockfd;
    socklen_t addrlen;
    t_connection * conn;

    conn = x_zalloc(sizeof(t_connection) + priv_size);
    if (!conn) {
        return NULL;
    }

    if(4 == port->flag){
        //struct sockaddr_in sock_addr;
        addrlen = sizeof(struct sockaddr_in);
        sockfd = m_accept(port->sockfd, (struct sockaddr *)&conn->remote_addr, &addrlen);
        if (sockfd < 0) {
            return NULL;
        }

        //inet_ntop(AF_INET, &sock_addr.sin_addr, conn->src_ip, 64);
        //conn->src_port = ntohs(sock_addr.sin_port);

        conn = x_connection_alloc_by_accecpt(sockfd, &conn->remote_addr,priv_size);
        if (!conn) {
            goto x_connection_alloc_by_accecpt_error;
        }
    }else if(6 == port->flag){
        //struct sockaddr_in6 sock_addr;
        addrlen = sizeof(struct sockaddr_in6);
        sockfd = m_accept(port->sockfd, (struct sockaddr *)&conn->remote_addr_ipv6, &addrlen);
        if(sockfd < 0){
            return NULL;
        }

        //inet_ntop(AF_INET6, &sock_addr.sin6_addr, conn->src_ip, 64);
        //conn->src_port = ntohs(sock_addr.sin6_port);

        conn = x_connection_ipv6_alloc_by_accecpt(sockfd, &conn->remote_addr_ipv6, priv_size);
        if(!conn){
            goto x_connection_alloc_by_accecpt_error;
        }
    }else{
        return NULL;
    }

    return conn;
x_connection_alloc_by_accecpt_error:
    close(sockfd);
    return NULL;
}
