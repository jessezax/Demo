#include "libx.h"
#include "comm.h"

static bool comm_mysql_insert_op(t_sql_for_batch *sql_info, e_whether_end end)
{
    u64 db_id = 0;

    if(is_end == end)
    {
        if (sql_info->sql_len)
        {
            if (!comm_mysql_insert_action_execute(sql_info->sql,sql_info->sql_len,&db_id, 0, NULL))
            {
                goto error;
            }
            bzero(sql_info, sizeof(t_sql_for_batch));
        }
    }
    else
    {
        if ((sql_info->num > MAX_BATCH_NUM_IN_ONE_SQL))
        {
            if (!comm_mysql_insert_action_execute(sql_info->sql,sql_info->sql_len,&db_id, 0, NULL))
            {
                goto error;
            }
            bzero(sql_info, sizeof(t_sql_for_batch));
        }
    }

    return true;
error:
    bzero(sql_info, sizeof(t_sql_for_batch));
    return false;
}

static bool __comm_mysql_insert(t_handle_msg_info *handle_msg, t_sql_for_batch *sql_info)
{
    char    encode_str[4096]    = {0};
    u32     encode_len          = 0;

    t_action_add *add_info = (t_action_add *)handle_msg->data;

    encode_len = comm_base64_encode((u8 *)handle_msg, handle_msg->data_len + sizeof(t_handle_msg_info), encode_str);
    if (!encode_len)
    {
        return false;
    }

    if (!sql_info->sql_len)     ///< sql语句的起始位置
    {
        sql_info->sql_len += snprintf(sql_info->sql + sql_info->sql_len, MAX_SQL_LEN_FOR_BATCH - sql_info->sql_len,
                                    "insert into `plc_table`("
                                        "`user_id`,`rule_id`,`msg_data`) "
                                    "values ");
    }
    else
    {
        sql_info->sql_len += snprintf(sql_info->sql + sql_info->sql_len, MAX_SQL_LEN_FOR_BATCH - sql_info->sql_len, ",");
    }

    sql_info->sql_len += snprintf(sql_info->sql + sql_info->sql_len, MAX_SQL_LEN_FOR_BATCH - sql_info->sql_len,
                                    "('%u','%u','%.*s')",
                                    add_info->user_id,
                                    add_info->rule_id,
                                    encode_len, encode_str);
    sql_info->num++;

    return true;
}


u32 last_rule_id = 0;

bool comm_mysql_insert(void)
{
    u32 idx;
    t_handle_msg_info   *msg        = NULL;
    t_handle_msg_info   *tmp        = NULL;
    t_sql_for_batch     sql_info;

    memset(&sql_info, 0x0, sizeof(t_sql_for_batch));

    /* 1. 遍历所有 "未插入数据库的链表" */
    /* 2. 将取出的数据插入数据库 */
    /* 3. 将取出的数据从 "未插入数据库的链表" 中删除 */
    /* 4. 将取出的数据插入到 "已插入数据库的链表" 中 */
    for (idx = 0; idx < HASH_TABLE_SIZE; idx++)
    {
        xlist_for_each_entry_safe(msg, tmp, &g_list_head_out_mysql.list_head[idx], list_head)
        {
            t_action_add *add_info = (t_action_add *)msg->data;
            if (add_info->save)         ///< 需要重启保存
            {
                __comm_mysql_insert(msg, &sql_info);
                comm_mysql_insert_op(&sql_info, not_end);
            }

            if (1 != (add_info->rule_id - last_rule_id))
            {
                printf("last ruleid is %u\n cur rule_id is %u\n", last_rule_id, add_info->rule_id);
            }

            last_rule_id = add_info->rule_id;

            xlist_del(&msg->list_head);
        xlist_add_tail(&msg->list_head, &g_list_head_in_mysql.list_head[idx]);
        }
    }
    comm_mysql_insert_op(&sql_info, is_end);

    return true;
}

