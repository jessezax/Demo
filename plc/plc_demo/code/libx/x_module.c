#include "libx.h"

static t_xmodule * g_xmodules[X_MOD_NUM] = {NULL};

void __x_module_register(t_xmodule * xmod,unsigned char idx)
{
    if (idx >= X_MOD_NUM) {
        fprintf(stderr,"%s [%s] error : idx [%u] overflow\n",__func__,xmod->name,idx);
        goto error;
    }
    if (g_xmodules[idx]) {
        fprintf(stderr,"%s [%s] error : idx [%u] exist already\n",__func__,xmod->name,idx);
        goto error;
    }
    xmod->idx = idx;
    g_xmodules[idx] = xmod;
    return;
error:
    exit(0);
}

int __module_init(unsigned char idx)
{
    t_xmodule * xmod;

    if (idx >= X_MOD_NUM) {
        fprintf(stderr,"%s error : idx [%u] overflow\n",__func__,idx);
        goto error;
    }

    xmod = g_xmodules[idx];
    if (xmod) {
        if (xmod->init) {
            if (xmod->init(xmod) < 0) {
                fprintf(stderr,"%s [%s] error!!\n",__func__,xmod->name);
                goto error;
            }
        }
    }

    return 0;
error:
    return -1;
}

int module_init(void)
{
    u32 idx;
    for (idx = 0;idx < X_MOD_NUM;idx++) {
        if (__module_init(idx) < 0) {
            return -1;
        }
    }
    return 0;
}


int __module_uninit(unsigned char idx)
{
    t_xmodule * xmod;

    if (idx >= X_MOD_NUM) {
        fprintf(stderr,"%s error : idx [%u] overflow\n",__func__,idx);
        goto error;
    }

    xmod = g_xmodules[idx];
    if (xmod) {
        if (xmod->uninit) {
            if (xmod->uninit(xmod) < 0) {
                fprintf(stderr,"%s [%s] error!!\n",__func__,xmod->name);
                goto error;
            }
        }
    }

    return 0;
error:
    return -1;
}

int module_uninit(void)
{
    unsigned char idx;
    for (idx = 0;idx < X_MOD_NUM;idx++) {
        if (__module_uninit(idx) < 0) {
            return -1;
        }
    }
    return 0;
}

int __module_reconf(unsigned char idx)
{
    t_xmodule * xmod;

    if (idx >= X_MOD_NUM) {
        fprintf(stderr,"%s error : idx [%u] overflow\n",__func__,idx);
        goto error;
    }

    xmod = g_xmodules[idx];
    if (xmod) {
        if (xmod->reconf) {
            if (xmod->reconf(xmod) < 0) {
                fprintf(stderr,"%s [%s] error!!\n",__func__,xmod->name);
                goto error;
            }
        }
    }

    return 0;
error:
    return -1;
}

int module_reconf(void)
{
    unsigned char idx;
    for (idx = 0;idx < X_MOD_NUM;idx++) {
        if (__module_reconf(idx) < 0) {
            return -1;
        }
    }
    return 0;
}

int module_ctrl(unsigned int ctrl,void * arg,unsigned int arg_size)
{
    unsigned char idx;
    t_xmodule * xmod;

    idx = XMOD_CTRL_M(ctrl);
    if (idx >= X_MOD_NUM) {
        fprintf(stderr,"%s error : idx [%u] overflow\n",__func__,idx);
        goto error;
    }

    xmod = g_xmodules[idx];
    if (!xmod) {
        fprintf(stderr,"%s error : idx [%u] not exsit\n",__func__,idx);
        goto error;
    }

    if (!xmod->ctrl) {
        fprintf(stderr,"%s [%s] error : doesn't support ctrl\n",__func__,xmod->name);
        goto error;
    }

    if (xmod->ctrl(xmod,ctrl,arg,arg_size) < 0) {
        fprintf(stderr,"%s [%s] error!!\n",__func__,xmod->name);
        goto error;
    }

    return 0;
error:
    return -1;
}
