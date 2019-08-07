#ifndef __comm_CONNECTION_H__
#define __comm_CONNECTION_H__

/**
 * @addtogroup comm
 * @{
 */


/**
 * @brief 策略协议TCP连接结构体
 */
typedef struct {
    t_connection        * connection;               ///< TCP连接句柄
    u32                 msg_idx;                    ///< 当前消息ID
    u8                  mysql_freq;                 ///< mysql操作频率
    u32                 last_mysql;                 ///< 最后一次发送mysql操作的时间
    struct xlist_head   timer_task_list;            ///< 用于定时任务链表
}t_comm_connection_priv;

///< 通过t_connection结构指针获得t_comm_connection_priv结构指针
#define comm_connection_priv(conn) x_connection_priv(t_comm_connection_priv, conn)

///< 初始化t_comm_connection_priv结构
void comm_connection_priv_init(t_connection * conn);

///< 申请由accept函数接收的priv为t_comm_connection_priv的t_connection结构
t_connection * comm_connection_alloc_by_accept(int sockfd,struct sockaddr_in * remote_addr);
t_connection * comm_ipv6_connection_alloc_by_accecpt(int sockfd,struct sockaddr_in6 * remote_addr);

///< 申请priv为t_comm_connection_priv的t_connection结构
t_connection * comm_connection_alloc(struct sockaddr_in * remote_addr);
t_connection * comm_ipv6_connection_alloc(struct sockaddr_in6 * remote_addr);


///< 设置mysql 操作频率
void comm_set_mysql_freq(t_connection * conn,u32 sec);

/*! @} */

#endif
