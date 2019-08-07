#ifndef __X_CONNECTION_H__
#define __X_CONNECTION_H__

typedef struct {
    int                sockfd;
    t_mutex            mutex;
    u8                 flag;                // 4表示ipv4，6表示ipv6
    u8                 user_id;
    union
    {
        struct sockaddr_in  remote_addr;
        struct sockaddr_in6 remote_addr_ipv6;
    };
    u8                 by_accept:1;
    u8                 connected:1;
    u8                 error:1;
    u8                 send_error:1;
    u8                 recv_error:1;
    u32                last_time;
    u32                msg_len;
    char              *list_heads[256];
    //char               src_ip[64];
    //u16                src_port;
    u8                 priv[0];
}__attribute__((__packed__)) t_connection;

#define x_connection_priv(type,conn) ((type *)((conn)->priv))

t_connection * x_connection_alloc_by_accecpt(int sockfd,struct sockaddr_in * remote_addr,u32 priv_size);
t_connection * x_connection_ipv6_alloc_by_accecpt(int sockfd,struct sockaddr_in6 * remote_addr,u32 priv_size);
t_connection * x_connection_alloc(struct sockaddr_in * remote_addr,u32 priv_size);
t_connection * x_connection_ipv6_alloc(struct sockaddr_in6 * remote_addr,u32 priv_size);

bool x_connection_connect(t_connection * conn);
void x_connection_colse(t_connection * conn);
void x_connection_free(t_connection * conn);

ssize_t x_connection_recv(t_connection * conn,void * buf,size_t size);
ssize_t x_connection_send(t_connection * conn,void * buf,size_t size);

#endif
