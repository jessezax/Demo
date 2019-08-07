#ifndef __PM_PLC_MYSQL_HANDLE_H__
#define __PM_PLC_MYSQL_HANDLE_H__

#define MAX_SQL_LEN_FOR_BATCH 40960
#define MAX_BATCH_NUM_IN_ONE_SQL 10

typedef enum
{
    not_end  = 0x00,
    is_end = 0x01
}e_whether_end;

// 批量插入SQL
typedef struct
{
    char    sql[MAX_SQL_LEN_FOR_BATCH];         ///< SQL语句
    int     sql_len;                            ///< SQL语句长度
    int     num;                                ///< 批量插入数量
}t_sql_for_batch;

/* MYSQL 删除操作 */
bool comm_mysql_del(void);

/* MYSQL 插入操作 */
bool comm_mysql_insert(void);

/* MYSQL 定时任务 */
void mysql_timer_task(void);

#endif

