#ifndef __comm_CONF_H__
#define __comm_CONF_H__

/**
 * @addtogroup comm
 * @{
 */

/**
 * @brief 主要配置项结构体
 */
typedef struct {
    time_t             start_time;          ///< 启动时间戳
    char               conf_file[256];      ///< 配置文件路径
    struct sockaddr_in local_addr;          ///< 本地绑定地址,供策略服务器主动连接
    struct sockaddr_in remote_addr;         ///< 策略服务器地址
}t_comm_conf;

///< 配置文件句柄
extern p_conf_ctx  g_conf_ctx;

///< 配置信息
extern t_comm_conf g_comm_conf;

/*! @} */

#endif
