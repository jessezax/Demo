/*!
 * @file    comm_mysql.h
 * @author  gc
 * @version 1.0
 * @date    2018/09/26
 * @brief   mysql模块
 * @details Copyright(c) 2018 All Rights Reserved.
 */


#ifndef __comm_MYSQL_H__
#define __comm_MYSQL_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <mysql/mysql.h>



#ifdef __cplusplus
extern "C" {
#endif

#define POOL_MAX_SIZE   100

// 连接节点
typedef struct
{
    MYSQL               fd;                 ///< Mysql对象文件描述符
    MYSQL               *mysql_sock;        ///< 指向已经连接的Mysql指针
    pthread_mutex_t     lock;               ///< 互斥锁
    int                 used;               ///< 使用标记
    int                 index;              ///< 下标
    enum                                    ///< 连接状态
    {
        DB_DISCONN,
        DB_CONN
    }db_state;
}t_db_conn_node;

// sql 连接池
typedef struct
{
    int             shutdown;                   ///< 是否关闭
    int             node_number;                ///< 连接节点数量
    int             busy_number;                ///< 已经被使用的连接数量
    t_db_conn_node  db_node[POOL_MAX_SIZE];     ///< 连接
}t_db_conn_pool;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Module macros
typedef bool (*SELECT_CALLBACK_FUNCTION) (int column,MYSQL_ROW l_row,MYSQL *conn,void * arg);



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Module variable

t_db_conn_pool *g_pool;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Module function

/**
* mysql_begin - 执行事务开始
*/
//bool comm_mysql_begin(void);

/**
* mysql_commit - 执行事务提交
*/
//bool comm_mysql_commit(void);

/**
* mysql_rollback - 执行事务回滚
*/
//bool comm_mysql_rollback(void);

bool comm_mysql_insert_action_execute(char *sql,int sql_len,u64 * insert_id, u32 content_len, u8 *content);

bool comm_mysql_select_action_execute(const char *sql,
                                        int sql_len,
                                        SELECT_CALLBACK_FUNCTION select_func,
                                        void * arg);

//bool comm_mysql_close(void);
#ifdef __cplusplus
}
#endif

#endif/*RSN_MYSQL_H*/
