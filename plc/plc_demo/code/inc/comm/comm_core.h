#ifndef __comm_CORE_H__
#define __comm_CORE_H__

///< 默认心跳频率
#define DEFAULT_MYSQL_FREQ  (10)
#define ACK_PLC_NUM         (2000000)
//#define ACK_PLC_NUM           (1)

extern u32 g_max_plc_num_for_ack;

///< 模块启动索引
enum {
    XMOD_CONFIG_IDX             = 0,    ///< 配置文件模块
    XMOD_TIMER_TASK_IDX,                ///< 策略服务器连接定时任务执行模块
    XMOD_MYSQL_IDX,                     ///< 数据库模块

    XMOD_RULE_FILE_IDX,                 ///< 规则文件写入模块
    XMOD_RULE_VERSION_IDX,              ///< 发布版本管理模块
    XMOD_NOTIFIER_IDX,                  ///< 消息通知模块

    XMOD_IP_PORT_PRO_IDX,               ///< 五元组规则处理模块
    XMOD_COND_CODE_IDX,                 ///< 特征码规则处理模块
    XMOD_ACCOUNT_IDX,                   ///< 宽带账号规则处理模块
    XMOD_PRO_APP_IDX,                   ///< 协议和应用规则处理模块
    XMOD_AUDIO_VIDEO_IDX,               ///< 音频和视频规则处理模块
    XMOD_HTTP_IDX,                      ///< HTTP规则处理模块
    XMOD_DNS_HOST_IDX,                  ///< DNS域名规则处理模块
    XMOD_MODEL_IDX,                     ///< 模型规则处理模块
    XMOD_CMB_RULE_IDX,                  ///< 组合规则出来模块

    XMOD_comm_CORE_IDX
};

t_tcp_port  g_notify_dpi_port;

extern  u64 g_max_rule_num;                     ///< 最大规则数，当超过当前规则数的时候，生成增量规则文件
extern  u64 g_max_increase_num;                 ///< 增量更新的最大规则数，大于此值时，执行全量更新
extern  u32 g_increase_freq;                    ///< 规则生成频率
extern  u8  g_log_flag;                         ///< 日志开关
extern  u32 g_rule_count;

char    g_user_name[256];                       ///< 用户名
char    g_passwd[256];                          ///< 密码

///< 判断缓冲区剩余空间并向缓冲区写数据
#define RULE_PUT(pos,tail,err_label,f,...) \
do { \
    pos += snprintf(pos,tail - pos,f,##__VA_ARGS__); \
    if (pos > tail) { \
        goto err_label; \
    } \
}while(0)

#endif
