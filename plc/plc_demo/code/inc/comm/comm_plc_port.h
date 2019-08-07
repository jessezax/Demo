#ifndef __comm_PLC_PORT_H__
#define __comm_PLC_PORT_H__


typedef struct{
    int                     sock_fd;
    struct sockaddr_in      bind_addr;
    struct ip_mreq          mreq;
    comm_u64                devid;
    time_t                  last_keepalive;
} __attribute__((__packed__)) t_comm_plc_port;

t_comm_plc_port *comm_plc_port_new(char * maddr, comm_u16 port);
void comm_plc_port_free(t_comm_plc_port * plc_port);

bool send_notify_msg(void *msg, u32 size);
bool comm_plc_port_recv(t_comm_plc_port * plc_port,t_comm_plc_msg ** __msg,struct sockaddr_in * remote_addr);
bool comm_plc_port_send(t_comm_plc_port * plc_port, t_comm_plc_msg * msg, struct sockaddr_in * remote_addr);

#endif
