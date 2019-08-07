#include "libx.h"
#include "comm.h"

typedef struct {
    char hostname[64];
    char username[64];
    char passwd[64];
    char database[64];
    u16  port;
}t_comm_mysql_conf;

static u32 g_pool_size  = 10;

//static __thread MYSQL * comm_mysql_conn = NULL;
//static __thread u32     comm_mysql_last_ping_clock = 0;
static t_comm_mysql_conf g_comm_mysql_conf;

static bool comm_mysql_reload(void)
{
    if (!x_conf_get_value(g_conf_ctx,(void*)g_comm_mysql_conf.hostname,XCONF_STRING,64,"root/db/host/value")) {
        return false;
    }
    if (!x_conf_get_value(g_conf_ctx,(void*)g_comm_mysql_conf.username,XCONF_STRING,64,"root/db/user/value")) {
        return false;
    }
    if (!x_conf_get_value(g_conf_ctx,(void*)g_comm_mysql_conf.passwd,XCONF_STRING,64,"root/db/passwd/value")) {
        return false;
    }
    if (!x_conf_get_value(g_conf_ctx,(void*)g_comm_mysql_conf.database,XCONF_STRING,64,"root/db/database/value")) {
        return false;
    }
    if (!x_conf_get_value(g_conf_ctx,(void*)&g_comm_mysql_conf.port,XCONF_U2,0,"root/db/port/value")) {
        return false;
    }
    x_conf_get_value(g_conf_ctx,(void*)&g_pool_size,XCONF_U4,0,"root/db/pool_size/value");

    return true;
}

#if 0
static bool __comm_mysql_connect(void)
{
    const char  *hostname       = g_comm_mysql_conf.hostname;
    const char  *username       = g_comm_mysql_conf.username;
    const char  *passwd         = g_comm_mysql_conf.passwd;
    const char  *database       = g_comm_mysql_conf.database;
    u16         port            = g_comm_mysql_conf.port;
    int         read_timeout    = 10;
    char        value           = 1;

    comm_mysql_conn = mysql_init(NULL);
    if(!comm_mysql_conn) {
        return false;
    }

    //设置mysql读超时
    mysql_options(comm_mysql_conn,MYSQL_OPT_READ_TIMEOUT,&read_timeout);

    //设置自动重连
    mysql_options(comm_mysql_conn, MYSQL_OPT_RECONNECT, (char *)&value);

    if(!mysql_real_connect(comm_mysql_conn,hostname,username,passwd,database,port,NULL,CLIENT_MULTI_STATEMENTS) ) {
        mysql_close(comm_mysql_conn);
        comm_mysql_conn = NULL;
        return false;
    }
    return true;
}

static bool comm_mysql_connect(void)
{
    if(!comm_mysql_conn) {
        return __comm_mysql_connect();
    }
    if (clock_monotonic() - comm_mysql_last_ping_clock >= 5) {
        if(mysql_ping(comm_mysql_conn) != 0) {
            mysql_close(comm_mysql_conn);
            comm_mysql_conn = NULL;
            return __comm_mysql_connect();
        }
        comm_mysql_last_ping_clock = clock_monotonic();
    }
    return true;
}

bool comm_mysql_close(void)
{
    if(comm_mysql_conn)
    {
        mysql_close(comm_mysql_conn);
    }
    return true;
}

/**
* comm_mysql_begin - 执行事务开始
* @mysql_conn
*/

bool comm_mysql_begin(void)
{
    int ret = 0;

    if (!comm_mysql_connect()) {
        return false;
    }

    ret = mysql_real_query(comm_mysql_conn, "start transaction;",const_strlen("start transaction;"));
    if(ret) {
        fprintf(stderr,"[%s] mysql query failure.(%s)!\n",__FUNCTION__,mysql_error(comm_mysql_conn));
        return false;
    }
    return true;
}

/**
* comm_mysql_commit - 执行事务提交
* @mysql_conn
*/
bool comm_mysql_commit(void)
{
    if(!comm_mysql_connect()) {
        return false;
    }
#if 0
    int ret = 0;
    ret = mysql_real_query(comm_mysql_conn, "commit;",const_strlen("commit;"));
    if(ret) {
        fprintf(stderr,"[%s] mysql query failure.(%s)!\n",__FUNCTION__,mysql_error(mysql_connect));
        return false;
    }
#else
    if (mysql_commit(comm_mysql_conn) != 0) {
        return false;
    }
#endif
    return true;
}

/**
* comm_mysql_rollback - 执行事务回滚
* @mysql_conn
*/

bool comm_mysql_rollback(void)
{
    if(!comm_mysql_connect()) {
        return false;
    }
#if 0
    int ret = 0;
    ret = mysql_real_query(comm_mysql_conn, "rollback;",const_strlen("rollback;"));
    if(ret) {
        fprintf(stderr,"[%s] mysql query failure.(%s)!\n",__FUNCTION__,mysql_error(comm_mysql_conn));
        return false;
    }
#else
    if (mysql_rollback(comm_mysql_conn) != 0) {
        return false;
    }
#endif
    return true;
}

/**
* comm_mysql_insert_action_execute - 执行mysql插入语句,返回插入的id
* @sql
* @sql_len
* @insert_id
*/
bool comm_mysql_insert_action_execute(const char *sql,int sql_len,u64 * insert_id)
{
    int             ret         = 0;
    my_ulonglong    l_id        = 0;
    MYSQL_RES       *l_result   = NULL;

    if (!comm_mysql_connect()) {
        return false;
    }

    ret = mysql_real_query(comm_mysql_conn, sql, sql_len);
    if(ret) {
        fprintf(stderr,"[%s] mysql query failure.(%s)!\n",__FUNCTION__,mysql_error(comm_mysql_conn));
        fprintf(stderr,"[%s] mysql sql error!\n",sql);
        return false;
    }
    l_id = mysql_insert_id(comm_mysql_conn);
    l_result = mysql_store_result(comm_mysql_conn);
    if(l_result) {
        mysql_free_result(l_result);
        l_result = NULL;
    }
    if (insert_id) {
        *insert_id = l_id;
    }
    return true;
}

/**
* comm_mysql_select_action_execute - 执行select语句查询返回结果,执行回调函数
* @sql
* @sql_len
* @conn
* @select_func 执行sql语句之后,对结果集进行处理的回调
*/
bool comm_mysql_select_action_execute(const char *sql,int sql_len,
                                        SELECT_CALLBACK_FUNCTION select_func,
                                        void * arg)
{
    int ret      = 0;
    int l_column = 0;
    MYSQL_RES *l_result = NULL;
    MYSQL_ROW l_row;

    if (!comm_mysql_connect()) {
        return false;
    }

    ret = mysql_real_query(comm_mysql_conn,sql,sql_len);
    if(ret) {
        return false;
    }
    l_result = mysql_store_result(comm_mysql_conn);
    if(l_result) {
        l_column = mysql_num_fields(l_result);
        while((l_row = mysql_fetch_row(l_result))) {
            if (!select_func(l_column,l_row,comm_mysql_conn,arg)) {
                break;
            }
        }
        mysql_free_result(l_result);
        l_result = NULL;
    }
    return true;
}
#endif

/**
 * @brief                       创建连接节点
 * @param[in]   pool            连接池
 * @param[in]   node            连接节点
 * @return                      创建成功，返回0；创建失败，返回-1
 */
bool db_node_create(t_db_conn_pool *pool, t_db_conn_node *node)
{
    int         over_time       = 1;        ///< 自动重连时间
    int         read_timeout    = 10;       ///< 读超时
    const char  *hostname       = g_comm_mysql_conf.hostname;
    const char  *username       = g_comm_mysql_conf.username;
    const char  *passwd         = g_comm_mysql_conf.passwd;
    const char  *database       = g_comm_mysql_conf.database;
    u16         port            = g_comm_mysql_conf.port;

    if(1 == pool->shutdown)
    {
        return false;
    }
    pthread_mutex_init(&node->lock, NULL);
    if(!mysql_init(&node->fd))
    {
        printf("db_node_create error when mysql_init.\n");
        return false;
    }

    node->mysql_sock = mysql_real_connect(&node->fd, hostname,username,passwd,database,port,NULL,CLIENT_MULTI_STATEMENTS);
    if(!node->mysql_sock)
    {
        node->db_state = DB_DISCONN;
        printf("db_node_create error when mysql_real_connect.\n");
        return false;
    }

    node->used      = 0;
    node->db_state  = DB_CONN;
    mysql_options(&node->fd, MYSQL_OPT_RECONNECT, &over_time);
    mysql_options(&node->fd,MYSQL_OPT_READ_TIMEOUT,&read_timeout);
    return true;
}

/**
 * @brief                       销节点
 * @param[in]   node            连接节点
 * @return
 */
void db_node_destroy(t_db_conn_node *node)
{
    mysql_close(node->mysql_sock);
    node->mysql_sock = NULL;
    node->db_state = DB_DISCONN;
}

/**
 * @brief                       归还连接节点
 * @param[in]   pool            连接池
 * @param[in]   node            连接节点
 * @return
 */
void db_node_release(t_db_conn_pool *pool, t_db_conn_node *node)
{
    node->used  = 0;
    pthread_mutex_unlock(&node->lock);
}

/**
 * @brief                       销毁连接池
 * @param[in]   pool            连接池
 * @return
 */
void db_pool_destroy(t_db_conn_pool *pool)
{
    u32     idx;

    pool->shutdown = 1;
    for(idx=0; idx<pool->node_number; idx++)
    {
        if(pool->db_node[idx].mysql_sock)
        {
            db_node_destroy(&pool->db_node[idx]);
        }
    }
    free(pool);
}

/**
 * @brief                       创建连接池
 * @param[in]   pool_size       连接池大小
 * @return                      创建的连接池指针
 */
t_db_conn_pool *db_pool_create(u32 pool_size)
{
    u32             idx;
    t_db_conn_pool  *pool   = NULL;

    // 判断连接数量是否合法
    if(pool_size < 1)
    {
        printf("mysql connect pool size is too small. default 1\n");
        return NULL;
    }
    if(pool_size > POOL_MAX_SIZE)
    {
        pool_size = POOL_MAX_SIZE;
    }

    pool = (t_db_conn_pool *)malloc(sizeof(t_db_conn_pool) + 256);
    if(!pool)
    {
        printf("db_pool_ctreate error when malloc pool.\n");
        return NULL;
    }
    memset(pool, 0x0, sizeof(t_db_conn_pool) + 256);

    for(idx=0; idx<pool_size; idx++)
    {
        if(!db_node_create(pool, &pool->db_node[idx]))
        {
            db_pool_destroy(pool);
            return NULL;
        }
        pool->db_node[idx].index    = idx;
        pool->node_number++;
    }

    return pool;
}

t_db_conn_node *db_node_get_from_pool(t_db_conn_pool *pool)
{
    u32     start_idx;
    u32     idx, cur_idx = 0;
    u32     ping_ret;

    if(1 == pool->shutdown)
    {
        printf("mysql is shutdown!\n");
        return NULL;
    }

    srand((int)time(0));
    start_idx = rand() % pool->node_number;                         ///< 获取随机开始下标

    if(pool->busy_number >= pool->node_number)
    {
        return NULL;
    }

    for(idx=0; idx<pool->node_number; idx++)
    {
        cur_idx = (start_idx + idx) % pool->node_number;            ///< 获取实际的下标
        if(!pool->db_node[cur_idx].used)                            ///< 判断当前连接是否已经被使用了
        {
            pthread_mutex_lock(&pool->db_node[cur_idx].lock);       ///< 加锁

            if(DB_DISCONN == pool->db_node[cur_idx].db_state)       ///< 当前连接已断开
            {
                if(!db_node_create(pool, &(pool->db_node[cur_idx])))    ///< 重新连接数据库
                {
                    db_node_release(pool, &(pool->db_node[cur_idx]));
                    continue;
                }
            }

            // 检查mysql服务器是否关闭了连接
            ping_ret = mysql_ping(pool->db_node[cur_idx].mysql_sock);
            if(ping_ret)
            {
                printf("mysql ping error!\n");
                pool->db_node[cur_idx].db_state = DB_DISCONN;
                db_node_release(pool, &pool->db_node[cur_idx]);
            }
            else
            {
                pool->db_node[cur_idx].used = 1;
                break;
            }
        }
    }
    if(idx == pool->node_number)
    {
        return NULL;
    }

    //printf("the connect %u is used\n", cur_idx);

    pool->busy_number++;
    return &pool->db_node[cur_idx];
}

/**
* comm_mysql_insert_action_execute - 执行mysql插入语句,返回插入的id
* @sql
* @sql_len
* @insert_id
*/
bool comm_mysql_insert_action_execute(char *sql,int sql_len,u64 * insert_id, u32 content_len, u8 *content)
{
    int             ret         = 0;
    my_ulonglong    l_id        = 0;
    MYSQL_RES       *l_result   = NULL;
    t_db_conn_node  *conn       = NULL;
    char            *end        = NULL;

    conn = db_node_get_from_pool(g_pool);
    if(!conn)
    {
        printf("no available connect to mysql\n");
        return false;
    }

    if(content_len)
    {
        end = sql + sql_len;
        *end++ = '\'';
        end += mysql_real_escape_string(conn->mysql_sock, end, (char *)content, content_len);
        *end++ = '\'';
        *end++ = ')';
        *end++ = ';';
        sql_len += (unsigned int)(end - sql);
    }

    ret = mysql_real_query(conn->mysql_sock, sql, sql_len);
    if(ret) {
        fprintf(stderr,"[%s] mysql query failure.(%s)!\n",__FUNCTION__,mysql_error(conn->mysql_sock));
        fprintf(stderr,"[%s] mysql sql error!\n",sql);
        db_node_release(g_pool, conn);
        g_pool->busy_number--;
        return false;
    }
    l_id = mysql_insert_id(conn->mysql_sock);
    l_result = mysql_store_result(conn->mysql_sock);
    if(l_result) {
        mysql_free_result(l_result);
        l_result = NULL;
    }
    if (insert_id) {
        *insert_id = l_id;
    }
    db_node_release(g_pool, conn);
    g_pool->busy_number--;

    return true;
}

/**
* comm_mysql_select_action_execute - 执行select语句查询返回结果,执行回调函数
* @sql
* @sql_len
* @conn
* @select_func 执行sql语句之后,对结果集进行处理的回调
*/
bool comm_mysql_select_action_execute(const char *sql,int sql_len,
                                        SELECT_CALLBACK_FUNCTION select_func,
                                        void * arg)
{
    int             ret         = 0;
    int             l_column    = 0;
    MYSQL_RES       *l_result   = NULL;
    MYSQL_ROW       l_row;
    t_db_conn_node  *conn       = NULL;

    conn = db_node_get_from_pool(g_pool);
    if(!conn)
    {
        printf("no available connect to mysql\n");
        return false;
    }

    ret = mysql_real_query(conn->mysql_sock,sql,sql_len);
    if(ret) {
        fprintf(stderr,"[%s] mysql query failure.(%s)!\n",__FUNCTION__,mysql_error(conn->mysql_sock));
        fprintf(stderr,"[%s] mysql sql error!\n",sql);
        db_node_release(g_pool, conn);
        g_pool->busy_number--;
        return false;
    }
    l_result = mysql_store_result(conn->mysql_sock);
    if(l_result) {
        l_column = mysql_num_fields(l_result);
        while((l_row = mysql_fetch_row(l_result))) {
            if (!select_func(l_column,l_row,conn->mysql_sock,arg)) {
                //break;
                continue;
            }
        }
        mysql_free_result(l_result);
        l_result = NULL;
    }

    db_node_release(g_pool, conn);
    g_pool->busy_number--;

    return true;
}

static int comm_mysql_init(t_xmodule * xmod)
{
    if (!comm_mysql_reload()) {
        return -1;
    }

    g_pool = db_pool_create(g_pool_size);
    if(!g_pool)
    {
        printf("db_pool_create error\n");
        return -1;
    }

    return 0;
}

static int comm_mysql_uninit(t_xmodule * xmod)
{
    return 0;
}

static int comm_mysql_reconf(t_xmodule * xmod)
{
    return 0;
}

static int comm_mysql_ctrl(t_xmodule * xmod,unsigned int ctrl,void * arg,unsigned int arg_size)
{
    return 0;
}

static  t_xmodule comm_xmod_mysql = {
    .name = "comm mysql",
    .idx = XMOD_MYSQL_IDX,
    .init = comm_mysql_init,
    .uninit = comm_mysql_uninit,
    .reconf = comm_mysql_reconf,
    .ctrl = comm_mysql_ctrl,
    .ctx = NULL
};

x_module_register(&comm_xmod_mysql,XMOD_MYSQL_IDX)
