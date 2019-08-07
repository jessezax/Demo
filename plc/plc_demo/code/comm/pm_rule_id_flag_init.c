#include "libx.h"
#include "comm.h"

t_hash_list_heads g_list_head_in_mysql;     ///< 存储已经存入数据库中的策略数据
t_hash_list_heads g_list_head_out_mysql;    ///< 存储待存储入数据库中的策略数据
t_hash_list_heads g_list_del_for_mysql;     ///< 存储待从数据库中删除的数据

rsn_log_ctx     *g_plc_log_ctx;             ///< 全局日志上下文信息
char            g_plc_log_path[256] = "/var/log/m_plc.log";

/**
 * @brief                   解析从数据库中取出的数据，并添加到账号链表中
 * @param[in]  column       每行中的个数
 * @param[in]  l_row        每行中每列对应值
 * @param[in]  conn         MYSQL连接
 * @param[in]  arg          参数
 * @return                  bool, 成功/失败
 */
static bool __load_info_from_mysql(int column, MYSQL_ROW l_row, MYSQL *conn, void *arg)
{
    char encode_str[4096] = {0};
    char decode_str[3072] = {0};
    u32  decode_len       = 0;

    t_handle_msg_info *msg = (t_handle_msg_info *)decode_str;
    t_handle_msg_info *handle_msg = NULL;
    t_action_add *add_info = (t_action_add *)msg->data;

    if (column < 1)
    {
        return false;
    }

    if (l_row[0])
    {
        memcpy(encode_str, l_row[0], strlen(l_row[0]));
    }

    decode_len = comm_base64_decode(encode_str, strlen(encode_str), decode_str);
    if (!decode_len)
    {
        printf("base64 decode failed!\n");
        return false;
    }

    if (decode_len < (sizeof(t_handle_msg_info) + msg->data_len))
    {
        return false;
    }

    handle_msg = x_zalloc(decode_len + 1);
    if (!handle_msg)
    {
        return NULL;
    }

    memcpy(handle_msg, decode_str, decode_len);

    pm_plc_data_insert(add_info->user_id, add_info->rule_id, handle_msg, mysql_in_list);

    return true;
}


static bool load_info_from_mysql(void)
{
    char sql[1024] = {0};
    u32 sql_len;

    sql_len = snprintf(sql, 1024, "select "
                                        "`msg_data` "
                                  "from `plc_table` "
                                        "order by `rule_id`;");

    if(!comm_mysql_select_action_execute(sql, sql_len, __load_info_from_mysql, NULL))
    {
        return false;
    }

    return true;
}

bool rule_id_flag_init(void)
{
    int jdx;

    x_conf_get_value(g_conf_ctx,(void*)g_plc_log_path,XCONF_STRING,64,"root/core/log_file/value");

    g_plc_log_ctx = rsn_log_open((const char *)g_plc_log_path, 50*1024*1024, RSN_LOG_DUMP|RSN_LOG_FLUSH);

    for (jdx = 0; jdx < HASH_TABLE_SIZE; jdx++)
    {
        INIT_XLIST_HEAD(&g_list_head_in_mysql.list_head[jdx]);
        INIT_XLIST_HEAD(&g_list_head_out_mysql.list_head[jdx]);
        INIT_XLIST_HEAD(&g_list_del_for_mysql.list_head[jdx]);
    }

    if(!load_info_from_mysql())
    {
        printf("load info from local mysql failed!\n");
    }

    return true;
}

