#include "libx.h"
#include "comm.h"

static struct xlist_head g_timer_task_head = XLIST_HEAD_INIT(g_timer_task_head);
static t_mutex           g_timer_task_mutex;
static pthread_t         g_timer_task_pid;

void comm_timer_task_add(t_connection * conn)
{
    t_comm_connection_priv * priv = comm_connection_priv(conn);
    mutex_lock(&g_timer_task_mutex);
    xlist_add_head(&priv->timer_task_list,&g_timer_task_head);
    mutex_unlock(&g_timer_task_mutex);
}

void comm_timer_task_del(t_connection * conn)
{
    t_comm_connection_priv * priv = comm_connection_priv(conn);
    mutex_lock(&g_timer_task_mutex);
    xlist_del(&priv->timer_task_list);
    mutex_unlock(&g_timer_task_mutex);
}

static void * timer_task_thread(void * arg)
{
    t_connection * conn;
    t_comm_connection_priv * priv;
    pthread_detach(pthread_self());
    while (true) {
        mutex_lock(&g_timer_task_mutex);
        xlist_for_each_entry(priv, &g_timer_task_head, timer_task_list) {
            conn = priv->connection;
            if (!conn->connected || conn->error) {
                continue;
            }
        }
        mutex_unlock(&g_timer_task_mutex);
        usleep(1000*1000);
    }
    pthread_exit(NULL);
}

static int comm_timer_task_init(t_xmodule * xmod)
{
    int ret;

    mutex_init(&g_timer_task_mutex);
    INIT_XLIST_HEAD(&g_timer_task_head);

    ret = pthread_create(&g_timer_task_pid,NULL,timer_task_thread,NULL);
    if(ret != 0){
        return -1;
    }
    return 0;
}

static int comm_timer_task_uninit(t_xmodule * xmod)
{
    return 0;
}

static int comm_timer_task_reconf(t_xmodule * xmod)
{
    return 0;
}

static int comm_timer_task_ctrl(t_xmodule * xmod,unsigned int ctrl,void * arg,unsigned int arg_size)
{
    return 0;
}

static t_xmodule comm_xmod_timer_task = {
    .name = "comm connection timer task",
    .idx = XMOD_TIMER_TASK_IDX,
    .init = comm_timer_task_init,
    .uninit = comm_timer_task_uninit,
    .reconf = comm_timer_task_reconf,
    .ctrl = comm_timer_task_ctrl,
    .ctx = NULL
};

x_module_register(&comm_xmod_timer_task,XMOD_TIMER_TASK_IDX)
