#define _GNU_SOURCE
#include <sched.h>
#include "libx.h"
#include "comm.h"

static t_tcp_port   * g_comm_port = NULL;
static t_tcp_port   * g_comm_port_ipv6  = NULL;

static pthread_t g_port_thread_id;

static pthread_t g_port_ipv6_thread_id;

static pthread_t g_timer_task_thread_id;    ///< 定时任务线程ID
static t_mutex   g_timer_task_mutex;

//static struct sockaddr_in6    g_remote_ipv6_addr;
static struct sockaddr_in6  g_local_ipv6_addr;

//static char               addr_ipv6[128]      = "::1";
static u16                  g_port_ipv6         = 25322;
static u16                  g_port_ipv4         = 25321;

static char                 dpi_addr[128]       = "127.0.0.1";
static u16                  dpi_port            = 1024;

char                        dpi_inq_addr[128]   = "127.0.0.1";                  ///< 向DPI查询的IP地址
u16                         dpi_inq_port        = 10240;                        ///< 向DPI查询的端口

static int                  g_cpu_id            = 0;                            ///< 指定主线程CPU_ID

u32                         g_increase_freq     = 10000;                        ///< 规则生成频率
u64                         g_max_rule_num      = 10000;                        ///< 最大规则数，当超过当前规则数的时候，生成增量规则文件
u64                         g_max_increase_num  = 10000;                        ///< 增量更新的最大规则数，大于此值时，执行全量更新

//static u8                 g_server_ipv4_flag  = 0;                            ///< 是否开启IPv4的服务端认证
//static u8                 g_server_ipv6_flag  = 0;                            ///< 是否开启IPv6的服务端认证
static u8                   g_local_ipv4_flag   = 0;                            ///< 是否开启IPv4的本地端口监听
static u8                   g_local_ipv6_flag   = 0;                            ///< 是否开启IPv6的本地端口监听
static u32                  g_full_update_freq  = 1000;                         ///< 全量更新频率
static u32                  g_mysql_freq        = 10;                           ///< mysql操作频率
static u64                  g_last_update_time  = 0;                            ///< 上次最后接收到规则的时间
static u64                  g_last_mysql_time   = 0;                            ///< 上次MySQL操作的时间

u8                          g_log_flag          = 0;                            ///< 全局日志记录开关
u64                         g_rule_start_idx    = 0;                            ///< 全量更新规则ID起始值
u64                         g_rule_idx          = 0;                            ///< 全量更新规则ID
u64                         curr_increase_rule_num;                             ///< 记录更新当前收到的规则数
u64                         curr_full_rule_num;                                 ///< 当前需要全量更新的规则总数

t_cmb_flag                  g_cmb_flag;                                         ///< 多匹配项组合规则是否单独生成文件

///< 注册设备信息配置
static bool comm_core_reload(void)
{
    curr_increase_rule_num = 0;
    curr_full_rule_num = 0;

    /* 日志开关 */
    x_conf_get_value(g_conf_ctx,(void*)&g_log_flag,XCONF_U1,0,"root/core/log/value");

    /* CPU_ID */
    x_conf_get_value(g_conf_ctx,(void*)&g_cpu_id,XCONF_U1,0,"root/core/cpu_id/value");

    /* 本地IPv4配置 */
    x_conf_get_value(g_conf_ctx,(void*)&g_port_ipv4,XCONF_U2,0,"root/local_server/port");
    g_comm_conf.local_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    g_comm_conf.local_addr.sin_port = htons(g_port_ipv4);
    g_comm_conf.local_addr.sin_family = AF_INET;

    /* 本地IPv6配置 */
    x_conf_get_value(g_conf_ctx,(void*)&g_port_ipv6,XCONF_U2,0,"root/local_server/port_v6");
    g_local_ipv6_addr.sin6_addr = in6addr_any;
    g_local_ipv6_addr.sin6_port = htons(g_port_ipv6);
    g_local_ipv6_addr.sin6_family = AF_INET6;


    /* 获取本地通知DPI更新规则的配置 */
    /* 端口和IP，默认端口1024，默认IP127.0.0.1 */
    x_conf_get_value(g_conf_ctx,(void*)dpi_addr,XCONF_STRING,64,"root/dpi/addr");
    x_conf_get_value(g_conf_ctx,(void*)&dpi_port,XCONF_U2,0,"root/dpi/port");
    if(0 == inet_pton(AF_INET, dpi_addr, &g_notify_dpi_port.local_addr.sin_addr))
    {
        return false;
    }
    g_notify_dpi_port.local_addr.sin_port = htons(dpi_port);
    g_notify_dpi_port.local_addr.sin_family = AF_INET;


    /* 获取和DPI通信，查询命中次数的配置*/
    /* 默认端口1024，默认IP127.0.0.1 */
    x_conf_get_value(g_conf_ctx,(void*)dpi_inq_addr,XCONF_STRING,64,"root/dpi_inq/addr");
    x_conf_get_value(g_conf_ctx,(void*)&dpi_inq_port,XCONF_U2,0,"root/dpi_inq/port");

    ///< 获取用户认证信息
    x_conf_get_value(g_conf_ctx,(void*)g_user_name,XCONF_STRING,256,"root/user_info/name/value");
    x_conf_get_value(g_conf_ctx,(void*)g_passwd,XCONF_STRING,256,"root/user_info/passwd/value");

    ///< 获取IPv4，IPv6开关信息
    x_conf_get_value(g_conf_ctx,(void*)&g_local_ipv4_flag,XCONF_U1,0,"root/local/ipv4/value");
    x_conf_get_value(g_conf_ctx,(void*)&g_local_ipv6_flag,XCONF_U1,0,"root/local/ipv6/value");

    memset(&g_cmb_flag, 0x0, sizeof(t_cmb_flag));

    x_conf_get_value(g_conf_ctx,(void*)&g_cmb_flag.comm_flag,XCONF_U1,0,"root/core/cmb_flag/comm/value");
    x_conf_get_value(g_conf_ctx,(void*)&g_cmb_flag.flow_flag,XCONF_U1,0,"root/core/cmb_flag/flow/value");


    /* 增量/全量配置 */
    x_conf_get_value(g_conf_ctx,(void*)&g_increase_freq,XCONF_U4,0,"root/notify/increase_freq/value");
    x_conf_get_value(g_conf_ctx,(void*)&g_full_update_freq,XCONF_U4,0,"root/notify/full_freq/value");
    x_conf_get_value(g_conf_ctx,(void*)&g_max_rule_num,XCONF_U8,0,"root/notify/max_full_num/value");
    x_conf_get_value(g_conf_ctx,(void*)&g_max_increase_num,XCONF_U8,0,"root/notify/max_increase_rule_num/value");
    x_conf_get_value(g_conf_ctx,(void*)&g_rule_start_idx,XCONF_U8,0,"root/notify/rule_idx/value");

    g_rule_idx = g_rule_start_idx;

    return true;
}

static unsigned int get_time(void)
{
    time_t t;
    t = time(NULL);
    unsigned int clock = time(&t);
    return clock;
}

static u64  start = 0;
static u64  end   = 0;
u32         g_rule_count = 0;

static void * connection_thread(void * arg)
{
    t_connection * conn = arg;
    u8 reconnect_cnt = 0;
    t_comm_msg * msg;
    u64 clock;

    if(g_cpu_id)
    {
        cpu_set_t mask;
        CPU_ZERO(&mask);
        CPU_SET(g_cpu_id, &mask);
        if (sched_setaffinity(0, sizeof(cpu_set_t), &mask) < 0) {
            printf("Error: setaffinity()\n");
            exit(0);
        }
        else
        {
            printf("sched_setaffinity success\n");
        }
    }
    g_rule_count = 0;
    start = get_time();
    printf("start is %lu\n", start);

    pthread_detach(pthread_self());
    while (true) {
        clock = clock_monotonic();
        if (!conn->connected) {
            if (conn->by_accept) {
                break;
            }
            if (!x_connection_connect(conn)) {
                reconnect_cnt++;
                if (reconnect_cnt >= 3) {
                    sleep(3);
                    reconnect_cnt = 0;
                }
                continue;
            }
        }
        comm_timer_task_add(conn);
        while (true) {
            if (comm_msg_recv(conn, &msg)) {
                comm_msg_handle(conn, msg, NULL);

                if(g_rule_count == g_max_plc_num_for_ack)
                {
                    printf("success is %u\n", success);
                    break;
                }
                continue;
            }
            break;
        }
        comm_timer_task_del(conn);
        x_connection_colse(conn);
        g_rule_count = 0;

        // 全量更新
        if(curr_full_rule_num >= g_max_rule_num)
        {
            rule_generate();
            conn->last_time = clock;
        }

        if(curr_increase_rule_num)
        {
            t_rule_file_ctx ctx;
            rule_file_ctx_init(&ctx, file_inc);
            file_ctx_complete(&ctx, file_inc);
            file_ctx_update(&ctx, file_inc);
            rule_file_ctx_destroy(&ctx, file_inc);
        }
    }
    x_connection_free(conn);
    end = get_time();
    printf("%u\n", g_rule_count);
    printf("end is %lu\n", end);
    printf("spend time is %lu\n\n", end -start);
    //comm_mysql_close();
    pthread_exit(NULL);
}

static void * port_thread(void * arg)
{
    t_tcp_port      * port = arg;
    t_connection    * conn;
    int             ret;
    pthread_t       pid;

    pthread_detach(pthread_self());
    while (true) {
        conn = comm_port_accept(port);
        if (!conn) {
            continue;
        }

        ret = pthread_create(&pid,NULL,connection_thread,conn);
        if(ret != 0){
            x_connection_free(conn);
        }
    }
    pthread_exit(NULL);
}

static void * timer_task_thread(void * arg)
{
    u32             now_time;
    t_rule_file_ctx ctx;
    u8              flag = 0;

    g_last_update_time = get_time();

    pthread_detach(pthread_self());
    while (true) {
        now_time = get_time();
        mutex_lock(&g_timer_task_mutex);
        if ((now_time - g_last_mysql_time) > g_mysql_freq)
        {
            mysql_timer_task();
        }
        if ((now_time - g_last_update_time) > g_full_update_freq)
        {
            rule_file_ctx_init(&ctx, 0);
            if(get_file_size(ctx.tuple_file_increase->file_path))
            {
                flag = 1;
            }
            if(get_file_size(ctx.comm_file_increase->file_path))
            {
                flag = 1;
            }
            if(get_file_size(ctx.singel_file_increase->file_path))
            {
                flag = 1;
            }
            if(get_file_size(ctx.flow_file_increase->file_path))
            {
                flag = 1;
            }
            rule_file_ctx_destroy(&ctx, 0);

            if(flag)
            {
                rule_generate();
            }
            flag = 0;
            g_last_update_time = get_time();
        }
        mutex_unlock(&g_timer_task_mutex);
        usleep(1000*1000);
    }
    pthread_exit(NULL);
}

bool comm_init()
{
    int ret;

    rule_id_flag_init();

    srand(clock_monotonic_ns());

    if (!comm_core_reload())
    {
        return false;
    }

    if (!comm_appid_info_load())
    {
        //return false;
    }

    start = get_time();
    printf("start is %lu\n", start);

    rule_generate();
    end = get_time();
    printf("end is %lu\n", end);
    printf("spend time is %lu\n\n", end -start);

    ret = pthread_create(&g_timer_task_thread_id,NULL,timer_task_thread,NULL);
    if(ret != 0){
        printf("timer_task_thread create failed!\n");
        return false;
    }

    if(g_local_ipv4_flag)
        {
        g_comm_port = x_tcp_port_alloc(&g_comm_conf.local_addr);
        if (!g_comm_port) {
            return false;
        }

        ret = pthread_create(&g_port_thread_id,NULL,port_thread,g_comm_port);
        if(ret != 0){
            printf ("Create pthread error!\n");
            goto x_connection_alloc_error;
        }
        else
        {
            printf ("ipv4 port listen is ready!\n");
        }
    }

    if(g_local_ipv6_flag)
    {
        g_comm_port_ipv6 = x_tcp_port_ipv6_alloc(&g_local_ipv6_addr);
        if(!g_comm_port_ipv6 ){
            return false;
        }

        ret = pthread_create(&g_port_ipv6_thread_id,NULL,port_thread,g_comm_port_ipv6);
        if(ret != 0){
            printf ("Create pthread error!\n");
            goto x_connection_alloc_error_ipv6;
        }
        else
        {
            printf ("ipv6 port listen is ready!\n");
        }
    }

    #if 0
    if(g_server_ipv4_flag)
    {
        g_comm_conn = comm_connection_alloc(&g_comm_conf.remote_addr);
        if (!g_comm_conn) {
            goto port_thread_create_error;
        }
        ret = pthread_create(&g_conn_thread_id,NULL,connection_thread,g_comm_conn);
        if(ret != 0){
            printf ("Create pthread error!\n");
            goto port_thread_create_error;
        }
        else
        {
            printf ("ipv4 port connect is ready!\n");
        }
    }

    if(g_server_ipv6_flag)
    {
        g_comm_conn_ipv6 = comm_ipv6_connection_alloc(&g_remote_ipv6_addr);
        if (!g_comm_conn_ipv6) {
            goto port_thread_create_error_ipv6;
        }
        ret = pthread_create(&g_conn_ipv6_thread_id, NULL, connection_thread, g_comm_conn_ipv6);
        if(ret != 0){
            printf ("Create pthread error!\n");
            goto port_thread_create_error_ipv6;
        }
        else
        {
            printf ("ipv6 port connect is ready!\n");
        }
    }
    #endif

    return true;
#if 0
port_thread_create_error_ipv6:
    if(g_server_ipv6_flag && g_comm_conn_ipv6)
    {
        x_connection_free(g_comm_conn_ipv6);
    }
port_thread_create_error:
    if(g_server_ipv4_flag && g_comm_conn)
    {
        x_connection_free(g_comm_conn);
    }
#endif
x_connection_alloc_error_ipv6:
    if(g_local_ipv6_flag && g_comm_port_ipv6)
    {
        x_tcp_port_free(g_comm_port_ipv6);
    }
x_connection_alloc_error:
    if(g_local_ipv4_flag && g_comm_port)
    {
        x_tcp_port_free(g_comm_port);
    }
    return false;
}

static int comm_core_init(t_xmodule * xmod)
{
    users_info_init();
    return comm_init();
}

static int comm_core_uninit(t_xmodule * xmod)
{
    return 0;
}

static int comm_core_reconf(t_xmodule * xmod)
{
    return 0;
}

static int comm_core_ctrl(t_xmodule * xmod,unsigned int ctrl,void * arg,unsigned int arg_size)
{
    return 0;
}


static t_xmodule comm_xmod_core = {
    .name = "comm core",
    .idx = XMOD_comm_CORE_IDX,
    .init = comm_core_init,
    .uninit = comm_core_uninit,
    .reconf = comm_core_reconf,
    .ctrl = comm_core_ctrl,
    .ctx = NULL
};

x_module_register(&comm_xmod_core,XMOD_comm_CORE_IDX)
