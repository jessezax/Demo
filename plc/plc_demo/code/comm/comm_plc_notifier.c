#include "libx.h"
#include "comm.h"

#if 0
//#define comm_PLC_PORT_MPORT           2000
//#define comm_PLC_PORT_MADDR          "239.255.255.250"

static char   mult_addr[64]         = "239.255.255.250";
static u32    mult_port             = 2000;

static pthread_t         g_comm_plc_port_thread_id;
static t_comm_plc_port * g_comm_plc_port = NULL;

static void * comm_plc_port_thread(void * arg)
{
    t_comm_plc_port * port = arg;
    t_comm_plc_msg  * msg;
    struct sockaddr_in remote_addr;

    pthread_detach(pthread_self());
    while (true) {
        if (!comm_plc_port_recv(port,&msg, &remote_addr)) {
            continue;
        }
        comm_plc_msg_handle(port,msg,&remote_addr,NULL);
    }
    pthread_exit(NULL);
}
#endif

bool comm_plc_notify(e_comm_plc_type type,u32 version,e_comm_data_type data_type,void * data,int data_size)
{
    t_comm_plc_msg * msg;
    msg = plc_msg_alloc();
    if (!msg) {
        return false;
    }
    plc_msg_set_header(msg, plc_msg_code, type);
    plc_msg_set_header(msg, plc_msg_dst_devid, BROADCAST_DEVID);
    plc_msg_set_header(msg, plc_msg_cookie, rand64());
    plc_msg_append_dword(msg, version,network_order);
    plc_msg_append_byte(msg, data_type);
    plc_msg_append(msg, data,data_size);

    //return comm_plc_port_send(g_comm_plc_port,msg,&g_comm_plc_port->bind_addr);
    return send_notify_msg(msg, sizeof(t_comm_plc_msg) + data_size + 4 + 1);
}

#if 0
static bool comm_plc_init(void)
{
    if (!x_conf_get_value(g_conf_ctx,(void*)mult_addr,XCONF_STRING,64,"root/core/notifier/addr")) {
        //return false;
    }
    if (!x_conf_get_value(g_conf_ctx,(void*)&mult_port,XCONF_U4,64,"root/core/notifier/port")) {
        //return false;
    }
    int ret;
    g_comm_plc_port = comm_plc_port_new(mult_addr, mult_port);
    if (!g_comm_plc_port) {
        return false;
    }
    ret = pthread_create(&g_comm_plc_port_thread_id,NULL,comm_plc_port_thread,g_comm_plc_port);
    if(ret != 0) {
        printf ("Create pthread error!\n");
        return false;
    }
    return true;
}

static int comm_notifier_init(t_xmodule * xmod)
{
    if (!comm_plc_init()) {
        return -1;
    }
    return 0;
}

static int comm_notifier_uninit(t_xmodule * xmod)
{
    return 0;
}

static int comm_notifier_reconf(t_xmodule * xmod)
{
    return 0;
}

static int comm_notifier_ctrl(t_xmodule * xmod,unsigned int notifier,void * arg,unsigned int arg_size)
{
    return 0;
}


static t_xmodule comm_xmod_notifier = {
    .name = "comm notifier",
    .idx = XMOD_NOTIFIER_IDX,
    .init = comm_notifier_init,
    .uninit = comm_notifier_uninit,
    .reconf = comm_notifier_reconf,
    .ctrl = comm_notifier_ctrl,
    .ctx = NULL
};

x_module_register(&comm_xmod_notifier,XMOD_NOTIFIER_IDX)
#endif
