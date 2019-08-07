#include "libx.h"
#include "comm.h"

static bool __comm_mysql_del(t_handle_msg_info *handle_msg)
{
    char            sql[1024]       = {0};
    int             sql_len;
    u64             db_id;

    t_action_add    *add_info       = (t_action_add *)handle_msg->data;

    sql_len = snprintf(sql, 1024, "delete from `plc_table` "
                                    "where `rule_id`='%u' and "
                                    "`user_id`='%u'",
                                    add_info->rule_id,
                                    add_info->user_id);

    if(!comm_mysql_insert_action_execute(sql, sql_len, &db_id, 0, NULL))
    {
        return false;
    }

    return true;
}

bool comm_mysql_del(void)
{
    int                 idx;
    t_handle_msg_info   *msg        = NULL;
    t_handle_msg_info   *next       = NULL;

    for (idx = 0; idx < HASH_TABLE_SIZE; idx++)
    {
        xlist_for_each_entry_safe(msg, next, &g_list_del_for_mysql.list_head[idx], list_head)
        {
            __comm_mysql_del(msg);
            handle_msg_info_free(msg);
        }
    }

    return true;
}

