#include "libx.h"
#include "comm.h"

t_comm_conf g_comm_conf = {
    .conf_file = "./etc/comm.conf"
};
p_conf_ctx  g_conf_ctx;

static int comm_conf_init(t_xmodule * xmod) {
    p_conf_ctx *ctx= x_conf_load(g_comm_conf.conf_file);
    if (!ctx) {
        return -1;
    }
    g_conf_ctx = ctx;
    return 0;
}

static int comm_conf_uninit(t_xmodule * xmod)
{
    return 0;
}

static int comm_conf_reconf(t_xmodule * xmod)
{
    return 0;
}

static int comm_conf_ctrl(t_xmodule * xmod,unsigned int ctrl,void * arg,unsigned int arg_size)
{
    return 0;
}

static  t_xmodule comm_xmod_conf = {
    .name = "comm conf",
    .idx = XMOD_CONFIG_IDX,
    .init = comm_conf_init,
    .uninit = comm_conf_uninit,
    .reconf = comm_conf_reconf,
    .ctrl = comm_conf_ctrl,
    .ctx = NULL
};

x_module_register(&comm_xmod_conf,XMOD_CONFIG_IDX)
