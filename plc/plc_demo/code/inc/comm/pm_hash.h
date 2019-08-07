#ifndef __PM_HASH_H__
#define __PM_HASH_H__

#define HASH_TABLE_SIZE         2000000         ///< 策略的哈希表大小
#define PRO_APP_HASH_TABLE_SIZE 10000           ///< 协议和应用ID的哈希表大小

typedef enum
{
    mysql_in_list   = 0x00,     ///< 在数据库中
    mysql_out_list  = 0x01,     ///< 不在数据库中
    mysql_del_list  = 0x02,     ///< 待从数据库中删除
}e_mysql_state;

typedef struct
{
    pthread_mutex_t     mutex;
    struct xlist_head   list_head[HASH_TABLE_SIZE];
}t_hash_list_heads;

typedef struct
{
    struct xlist_head   list_head[PRO_APP_HASH_TABLE_SIZE];
}t_hash_list_heads_for_pro_app;

extern  t_hash_list_heads               g_list_head_in_mysql;
extern  t_hash_list_heads               g_list_head_out_mysql;
extern  t_hash_list_heads               g_list_del_for_mysql;
extern  t_hash_list_heads_for_pro_app   g_list_head_for_pro_app;

u32 pm_hash_func(u64 data, u32 size);

bool pm_pro_app_insert(t_pro_app_type_info *info);

t_handle_msg_info *pm_plc_data_search(u8 user_id, u32 rule_id);
bool pm_plc_data_insert(u8 user_id, u32 rule_id, t_handle_msg_info *msg_info, e_mysql_state state);
t_handle_msg_info *pm_plc_data_del(u8 user_id, u32 rule_id, e_handle_msg_type msg_type);

#endif
