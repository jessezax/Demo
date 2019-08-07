/*!
 * @file    rsn_log.h
 * @version 1.0
 * @brief   日志结构定义和接口函数，可以调用LOG直接显示在终端或记录到syslog，也可以调用n_log同时写入文件。
 * @details Copyright(c) 2018 All Rights Reserved.
 */


#ifndef _RSN_LOG_H
#define _RSN_LOG_H

/**
 * @addtogroup RSNLib
 * @{
*/

#ifdef __cplusplus
extern "C" {
#endif

////////////////////////////////////////////////////////////////////////////////

                         ///< PRI_EMERG,PRI_ALERT,PRI_CRIT,PRI_ERR日志会写入syslog
#define PRI_EMERG    0   /**< System is unusable.               */
#define PRI_ALERT    1U  /**< Action must be taken immediately. */
#define PRI_CRIT     2U  /**< Critical conditions.              */
#define PRI_ERR      3U  /**< Error conditions.                 */
                         ///< 以下优先级不会输出在syslog
#define PRI_WARN     4U  /**< Warning conditions.               */
#define PRI_NOTICE   5U  /**< Normal but significant condition. */
#define PRI_INFO     6U  /**< Informational.                    */
#define PRI_DEBUG    7U  /**< Debug-level messages.             */
#define PRI_MASK     0x07
#define LOG_STDOUT   0x08 ///< 终端显示掩码，是否在终端显示日志，可以和优先级or运算


#define RSN_LOG_DUMP  0x01    ///< 当日子文件超过设定大小则转储文件而不是覆盖
#define RSN_LOG_FLUSH 0x02    ///< 是否在每次写入后都flush


typedef struct st_log_ctx rsn_log_ctx;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*!
 * @brief   function    初始化日志文件，可以指定将日志文件路径.
 * @param   log_path    文件路径，相对路径或绝对路径；
 * @param   max_sz      文件最大大小，不能为0，超过此大小覆盖或转储(取决于flag的设定)；
 * @param   flags       RSN_LOG_*标记，可以是多个标记的OR组合；
 * @return  rsn_log_ctx* 日志上下文指针或NULL；
 */
extern rsn_log_ctx *rsn_log_open(const char *log_path, const u32 max_sz, const int flags);

/*!
 * @brief   function    释放日志上下文指针。
 * @param   logctx      @n_log_open创建的指针；
 * @return  void
 */
extern void     rsn_log_close(rsn_log_ctx *logctx);

/*!
 * @brief   function    记录文本日志信息到指定日志文件中，如果logctx为NULL只显示在终端或记录到syslog，不会记录到文件中。
 * @param   logctx      @n_log_open创建的指针，可以为NULL；
 * @param   pri         PRI_*优先级，0-3会记录到syslog，和LOG_STDOUT进行or运算会实时显示在终端
 * @param   fmt         日志文本的格式字符串，后面跟具体参数，具体格式可参考printf;
 * @return  void
 */
extern void     rsn_log(rsn_log_ctx *logctx, u8 pri, const char *fmt, ...);

/*!
 * @brief   function    记录日志，但不写入自定义日志文件，不需要任何初始化.
 * @param   pri         PRI_*优先级，小于或等于PRI_ERR的优先级会记录到syslog，其他只显示在终端部计入syslog；
 * @param   fmt         日志文本的格式字符串，后面跟具体参数，具体格式可参考printf;
 * @return  void
 */
#define LOG(pri, fmt, ...) rsn_log(NULL, pri, fmt, ##__VA_ARGS__);

__attribute__((__unused__))static char error_buf_[1024];

#define RSN_ERROR(err, fmt, ...)\
        { \
            sprintf(error_buf_, fmt, ##__VA_ARGS__);\
            *(err) = error_buf_;\
            return -1;\
        }

#define RSN_ERROR_P(err, fmt, ...)\
        { \
            sprintf(error_buf_, fmt, ##__VA_ARGS__);\
            *(err) = error_buf_;\
            return NULL;\
        }

#define RSN_EXIT(fmt, ...)\
        { \
            printf(fmt, ##__VA_ARGS__);\
            exit(-2);\
        }


#define RSN_WARN(fmt, ...)\
        { \
            printf(fmt, ##__VA_ARGS__);\
        }

#define RSN_INFO(fmt, ...)\
        { \
            printf(fmt, ##__VA_ARGS__);\
        }


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef __cplusplus
}
#endif

/*! @} */

#endif /* _RSN_LOG_H */
