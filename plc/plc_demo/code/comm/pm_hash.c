#include "libx.h"
#include "comm.h"

u32 pm_hash_func(u64 data, u32 size)
{
    return data % size;
}

bool pm_pro_app_insert(t_pro_app_type_info *info)
{
    u32 key;

    key = pm_hash_func(info->app_id, PRO_APP_HASH_TABLE_SIZE);

    /* 将账号信息添加到 未添加到数据库的账号信息链表 中 */
    xlist_add_tail(&info->list_head, &g_list_head_for_pro_app.list_head[key]);

    return true;
}

t_handle_msg_info *pm_plc_data_search(u8 user_id, u32 rule_id)
{
    u64 data;
    u32 key;

    bool                find_flag   = false;        ///< 标记是否已经找到
    t_handle_msg_info   *msg        = NULL;         ///< 当前消息完整结构
    t_handle_msg_info   *next       = NULL;         ///< 临时存储下一条信息

    data = user_id;
    data = (data << 32) + rule_id;

    key = pm_hash_func(data, HASH_TABLE_SIZE);

    /* 遍历已经插入数据库的账号信息链表,                 查找账号信息*/
    xlist_for_each_entry_safe(msg, next, &g_list_head_in_mysql.list_head[key], list_head)
    {
        /* 获取新增消息动作内容 */
        t_action_add *add_info = (t_action_add *)msg->data;

        if(add_info->rule_id == rule_id)            ///< 根据规则ID判断是否找到
        {
            find_flag = true;
            break;
        }
    }

    /* 在已经插入数据库的账号信息链表中未找到数据 */
    if(!find_flag)
    {
        /* 遍历未插入数据库的账号信息链表, 查找账号信息*/
        xlist_for_each_entry_safe(msg, next, &g_list_head_out_mysql.list_head[key], list_head)
        {
            /* 获取新增消息动作内容 */
            t_action_add *add_info = (t_action_add *)msg->data;

            if(add_info->rule_id == rule_id)        ///< 根据规则ID判断是否找到
            {
                find_flag = true;
                break;
            }
        }
    }

    if(find_flag)
    {
        return msg;
    }

    return NULL;
}

bool pm_plc_data_insert(u8 user_id, u32 rule_id, t_handle_msg_info *msg_info, e_mysql_state state)
{
    u64 data;
    u32 key;

    data = user_id;
    data = (data << 32) + rule_id;

    key = pm_hash_func(data, HASH_TABLE_SIZE);

    /* 将账号信息添加到 未添加到数据库的账号信息链表 中 */
    if (mysql_out_list == state)
    {
        xlist_add_tail(&msg_info->list_head, &g_list_head_out_mysql.list_head[key]);
    }
    else if (mysql_in_list == state)
    {
        xlist_add_tail(&msg_info->list_head, &g_list_head_in_mysql.list_head[key]);
    }
    else if (mysql_del_list == state)
    {
        xlist_add_tail(&msg_info->list_head, &g_list_del_for_mysql.list_head[key]);
    }

    return true;
}

t_handle_msg_info *pm_plc_data_del(u8 user_id, u32 rule_id, e_handle_msg_type type)
{
    u64 data;
    u32 key;
    bool                find_flag   = false;        ///< 标记是否已经找到
    t_handle_msg_info   *msg        = NULL;         ///< 当前消息完整结构
    t_handle_msg_info   *next       = NULL;         ///< 临时存储下一条信息

    data = user_id;
    data = (data << 32) + rule_id;

    key = pm_hash_func(data, HASH_TABLE_SIZE);

    pthread_mutex_lock(&g_list_head_in_mysql.mutex);
    pthread_mutex_lock(&g_list_head_out_mysql.mutex);
    pthread_mutex_lock(&g_list_del_for_mysql.mutex);

    /* 遍历已经插入数据库的账号信息链表,                 查找账号信息*/
    xlist_for_each_entry_safe(msg, next, &g_list_head_in_mysql.list_head[key], list_head)
    {
        /* 获取新增消息动作内容 */
        t_action_add *add_info = (t_action_add *)msg->data;

        if(add_info->rule_id == rule_id)            ///< 根据规则ID判断是否找到
        {
            /* 从 已经存入数据库的账号信息链表 中删除数据 */
            xlist_del(&msg->list_head);

            /* 将数据加入到 待删除的账号信息链表 中 */
            xlist_add_tail(&msg->list_head, &g_list_del_for_mysql.list_head[key]);
            find_flag = true;
            break;
        }
    }

    /* 在已经插入数据库的账号信息链表中未找到数据 */
    if(!find_flag)
    {
        /* 遍历未插入数据库的账号信息链表, 查找账号信息*/
        xlist_for_each_entry_safe(msg, next, &g_list_head_out_mysql.list_head[key], list_head)
        {
            /* 获取新增消息动作内容 */
            t_action_add *add_info = (t_action_add *)msg->data;

            if(add_info->rule_id == rule_id)        ///< 根据规则ID判断是否找到
            {
                /* 从 未存入数据库的账号信息链表 中删除数据 */
                xlist_del(&msg->list_head);

                /* 将数据加入到 待删除的账号信息链表 中 */
                xlist_add_tail(&msg->list_head, &g_list_del_for_mysql.list_head[key]);
                find_flag = true;
                break;
            }
        }
    }

    pthread_mutex_unlock(&g_list_head_in_mysql.mutex);
    pthread_mutex_unlock(&g_list_head_out_mysql.mutex);
    pthread_mutex_unlock(&g_list_del_for_mysql.mutex);

    if (find_flag)
    {
        return msg;
    }

    return NULL;
}

