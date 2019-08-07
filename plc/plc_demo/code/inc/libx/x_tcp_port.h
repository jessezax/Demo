#ifndef __X_TCP_PORT_H__
#define __X_TCP_PORT_H__

typedef struct {
    int sockfd;
    u8 flag;        // 4表示ipv4，6表示ipv6
    union
    {
        struct sockaddr_in local_addr;
        struct sockaddr_in6 local_addr_ipv6;
    };
}__attribute__((__packed__)) t_tcp_port;

t_tcp_port * x_tcp_port_alloc(struct sockaddr_in * local_addr);
t_tcp_port * x_tcp_port_ipv6_alloc(struct sockaddr_in6 * local_addr);

void x_tcp_port_free(t_tcp_port * port);

t_connection * x_tcp_port_accept(t_tcp_port * port,u32 priv_size);

#endif
