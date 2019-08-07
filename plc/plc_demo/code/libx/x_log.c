#include "libx.h"
#include "mxml.h"
#include <syslog.h>

static const char *s_log_pri[] =
{
    "[EMERG]",
    "[ALERT]",
    "[CRITI]",
    "[ERROR]",
    "[WARNING]",
    "[NOTIFY]",
    "[INFOR]",
    "[DEBUG]",
    NULL,
};

#define MONITOR_SECONDS 60
#define L_PATH 256

struct st_log_ctx
{
    char            path[L_PATH];
    FILE            *file;
    u64             max_size;
    u32             flag;
    pthread_t       monitor_thread;
    pthread_mutex_t mutex;
};
#if 0
static void log_close_forever(void)
{
    closelog();
}
#endif
static void *log_monitor_thread(void *para)
{
    struct stat log_stat;
    time_t curtime;
    struct tm curtm;
    char fname[L_PATH];
    rsn_log_ctx *logctx = para;

    while (true)
    {
        sleep(MONITOR_SECONDS);

        pthread_mutex_lock(&logctx->mutex);

        fflush(logctx->file);
        fstat(fileno(logctx->file), &log_stat);
        if ((u64)(log_stat.st_size) > logctx->max_size)
        {
            fclose(logctx->file);

            if (logctx->flag & RSN_LOG_DUMP)
            {
                time(&curtime);
                localtime_r(&curtime, &curtm);
                snprintf(fname, L_PATH, "%s_%04d_%02d_%02d_%02d_%02d_%02d", logctx->path, 1900 + curtm.tm_year, curtm.tm_mon + 1, curtm.tm_mday, curtm.tm_hour, curtm.tm_min, curtm.tm_sec);
                rename(logctx->path, fname);
            }

            while ((logctx->file = fopen(logctx->path, "w")) == NULL)
            {
                sleep(MONITOR_SECONDS);
            }
        }

        pthread_mutex_unlock(&logctx->mutex);
    }

    pthread_exit(NULL);
}

rsn_log_ctx *rsn_log_open(const char *log_path, const u32 max_sz, const int flag)
{
    rsn_log_ctx *logctx = malloc(sizeof(struct st_log_ctx));
    memset(logctx, 0, sizeof(struct st_log_ctx));

    if (log_path == NULL)
    {
        return NULL;
    }
    else
    {
        snprintf(logctx->path, L_PATH, "%s", log_path);
        logctx->max_size = max_sz;
        logctx->flag = flag;
    }

    logctx->file = fopen(logctx->path, "a+");
    if (logctx->file == NULL)
    {
        fprintf(stderr, "Open file err%s", strerror(errno));
        return NULL;
    }

    pthread_mutex_init(&logctx->mutex, NULL);

    if (pthread_create(&logctx->monitor_thread, NULL, log_monitor_thread, logctx) != 0)
    {
        logctx->monitor_thread = 0;
        return NULL;
    }

    openlog(NULL, LOG_CONS | LOG_PID, 0);

    return logctx;
}

void rsn_log_close(rsn_log_ctx *logctx)
{
    if (logctx)
    {
        pthread_cancel(logctx->monitor_thread);
        fclose(logctx->file);
    }
}

//logctx可以为NULL，就是不写入日志文件
void rsn_log(rsn_log_ctx *logctx, u8 pri, const char *fmt, ...)
{
    u8 in_stdout = pri & LOG_STDOUT;
    u8 pri_real  = pri & PRI_MASK;

    //所有日志必须在终端输出，但只有重要日志是在syslog&stderr输出，其他在stdout输出
    if (pri_real > PRI_ERR)
    {
        if (in_stdout)
        {
            va_list info_va;
            va_start(info_va, fmt);
            vfprintf(stdout, fmt, info_va);
            va_end(info_va);
        }
    }
    else
    {
        va_list err_va;
        va_start(err_va, fmt);
        vsyslog(pri_real, fmt, err_va);     //syslog
        va_end(err_va);

        if (in_stdout)
        {
            va_start(err_va, fmt);
            vfprintf(stdout, fmt, err_va);
            va_end(err_va);
        }
    }

    if (logctx)
    {
        time_t curtime;
        struct tm curtm;
        va_list file_va;

        time(&curtime);
        localtime_r(&curtime, &curtm);
        va_start(file_va, fmt);

        pthread_mutex_lock(&logctx->mutex);
        if (logctx->file)
        {
            fprintf(logctx->file, "%2.2d-%2.2d-%2.2d %2.2d:%2.2d:%2.2d %s: ",
                    curtm.tm_year + 1900,
                    curtm.tm_mon + 1,
                    curtm.tm_mday,
                    curtm.tm_hour,
                    curtm.tm_min,
                    curtm.tm_sec,
                    s_log_pri[pri_real]);
            vfprintf(logctx->file, fmt, file_va);

            if (logctx->flag & RSN_LOG_FLUSH)
            {
                fflush(logctx->file);
            }
        }
        pthread_mutex_unlock(&logctx->mutex);

        va_end(file_va);
    }
}

