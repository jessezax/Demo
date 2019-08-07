#include "libx.h"
#include "comm.h"
#include <sys/poll.h>

#define MSG_TTL    (128)

static void tcp_connect_ipv4(int *sock)
//static void tcp_connect_ipv4(int *sock, char *ipaddr, const unsigned short port)
{
    int err = -1;
    socklen_t addr_len = 0;

    *sock = socket(AF_INET, SOCK_STREAM, 0);
    if(-1 == *sock)
    {
        perror("socket error: ");
        return ;
    }

    addr_len = sizeof(struct sockaddr_in);
    err = connect(*sock, (struct sockaddr *)&g_notify_dpi_port.local_addr, addr_len);
    if(-1 == err)
    {
        perror("connect error:");
        close(*sock);
        return ;
    }
}

bool send_notify_msg(void *msg, u32 size)
{
    int sock = -1;
    //u16 port = 10240;
    //char  ipaddr[64] = {0};
    //strncpy(ipaddr, "127.0.0.1", strlen("127.0.0.1"));
    tcp_connect_ipv4(&sock);
    //tcp_connect_ipv4(&sock, ipaddr, port);
    send_all_size(sock, msg, size);
    close(sock);

    return true;
}

#if 1
void comm_plc_port_keepalive(t_comm_plc_port * port)
{
    time_t now;
    now = time(NULL);
    if (now - port->last_keepalive >= 30)
    {
        t_comm_plc_msg * msg = plc_msg_alloc();
        if (msg)
        {
            plc_msg_set_header(msg, plc_msg_code, comm_plc_heartbeat);
            plc_msg_set_header(msg, plc_msg_dst_devid, BROADCAST_DEVID);
            plc_msg_set_header(msg, plc_msg_src_devid, port->devid);
            plc_msg_set_header(msg, plc_msg_cookie, rand64());
            if (comm_plc_port_send(port,msg, &port->bind_addr))
            {
                port->last_keepalive = now;
            }
        }
    }
}

t_comm_plc_port *comm_plc_port_new(char * maddr, comm_u16 port)
{
    int                  ret;
    int                  ttl  = MSG_TTL;
    int                  loop = 0;
    t_comm_plc_port     *port_new;

    port_new = (t_comm_plc_port *)comm_zalloc(sizeof(t_comm_plc_port));
    if (!port_new)
    {
        return NULL;
    }
    port_new->devid = rand64();

    // socket的地址端口初始化
    port_new->bind_addr.sin_family              =   AF_INET;
    port_new->bind_addr.sin_port                =   comm_htons(port);
    port_new->bind_addr.sin_addr.s_addr         =   inet_addr(maddr);

    // 组播信息初始化
    port_new->mreq.imr_interface.s_addr         =   comm_htonl(INADDR_ANY);
    port_new->mreq.imr_multiaddr.s_addr         =   inet_addr(maddr);

    port_new->sock_fd = socket(AF_INET, SOCK_DGRAM, 0);     ///< 创建socket, sock_fd的初始化
    if (port_new->sock_fd < 0)
    {
        goto socket_error;
    }
    // 加入组播
    ret = setsockopt(port_new->sock_fd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &port_new->mreq, sizeof(port_new->mreq));
    if(ret < 0)
    {
        printf("setsockopt() IP_ADD_MEMBERSHIP error: %s(errno: %d)\n", strerror(errno), errno);
        goto setsockopt_error;
    }

    // 设置TTL
    ret = setsockopt(port_new->sock_fd, IPPROTO_IP, IP_MULTICAST_TTL, &ttl, ttl);
    if(ret < 0)
    {
        printf("setsockopt() IP_MULTICAST_TTL error: %s(errno: %d)\n", strerror(errno), errno);
        goto setsockopt_error;
    }

    ret = setsockopt(port_new->sock_fd, IPPROTO_IP, IP_MULTICAST_LOOP, &loop, sizeof(loop));
    if(ret < 0)
    {
        printf("setsockopt() IP_MULTICAST_LOOP error: %s(errno: %d)\n", strerror(errno), errno);
        goto setsockopt_error;
    }

    ret = bind(port_new->sock_fd, (struct sockaddr *)&port_new->bind_addr,  sizeof(port_new->bind_addr));
    if(ret < 0)
    {
        printf("bind() error: %s(errno: %d)\n", strerror(errno), errno);
        goto bind_error;
    }

    return port_new;
bind_error:
setsockopt_error:
    close(port_new->sock_fd);
socket_error:
    comm_free(port_new);
    return NULL;
}

void comm_plc_port_free(t_comm_plc_port * plc_port)
{
    int ret = setsockopt(plc_port->sock_fd, IPPROTO_IP, IP_DROP_MEMBERSHIP, (char *)&plc_port->mreq, sizeof(plc_port->mreq));
    if(ret < 0)
    {
        printf("setsockopt() IP_DROP_MEMBERSHIP error: %s(errno: %d)\n", strerror(errno), errno);
    }

    ret = close(plc_port->sock_fd);
    if(ret < 0)
    {
        printf("close socket error: %s(errno: %d)\n", strerror(errno), errno);
    }

    comm_free(plc_port);
}

bool comm_plc_port_recv(t_comm_plc_port * plc_port,t_comm_plc_msg ** __msg,struct sockaddr_in * remote_addr)
{
    int        ret;
    socklen_t  sock_len = sizeof(struct sockaddr_in);
    struct pollfd pfd;
    t_comm_plc_msg * msg = plc_msg_alloc();
    if (!msg) {
        return false;
    }
    while(true){
        comm_plc_port_keepalive(plc_port);
        pfd.fd      = plc_port->sock_fd;
        pfd.events  = POLLIN;
        pfd.revents = 0;
        if(comm_poll(&pfd, 1, 1000) > 0){
            ret = recvfrom(plc_port->sock_fd, msg, sizeof(t_comm_plc_msg) + comm_PLC_MSG_SIEZ, 0, (struct sockaddr *)remote_addr, &sock_len);
            if(ret < 0){
                if(errno == EINTR || errno == EAGAIN){
                    continue;
                }
                continue;
            }
            if(ret < (int)sizeof(t_comm_plc_msg)){
                continue;
            }
            if(msg->dst_devid != plc_port->devid && msg->dst_devid != BROADCAST_DEVID){
                continue;
            }
            msg->length = comm_ntohs(msg->length);
            if(msg->length > ret){
                continue;
            }
            *__msg = msg;
            return true;
        }
    }
    
#if 0
retry:
    ret = recvfrom(plc_port->sock_fd, msg, sizeof(t_comm_plc_msg) + comm_PLC_MSG_SIEZ, 0, (struct sockaddr *)remote_addr, &sock_len);
    if(ret < 0)
    {
        if (errno == EINTR || errno == EAGAIN)
        {
            goto retry;
        }
        plc_msg_free(msg);
        return false;
    }
    if (ret < sizeof(t_comm_plc_msg)) {
        goto retry;
    }
    if (msg->dst_devid != plc_port->devid && msg->dst_devid != BROADCAST_DEVID) {
        goto retry;
    }
    msg->length = ntohs(msg->length);
    if (msg->length > ret) {
        goto retry;
    }
    *__msg = msg;
    return true;
#endif
}

bool comm_plc_port_send(t_comm_plc_port * plc_port, t_comm_plc_msg * msg, struct sockaddr_in * remote_addr)
{
    int        ret;
    socklen_t  sock_len  = sizeof(struct sockaddr_in);
retry:
    ret = sendto(plc_port->sock_fd, msg, ntohs(msg->length), 0, (struct sockaddr *)remote_addr, sock_len);
    if(ret < 0)
    {
        if (errno == EINTR || errno == EAGAIN)
        {
            goto retry;
        }
        plc_msg_free(msg);
        return false;
    }
    plc_msg_free(msg);
    return true;
}
#endif
