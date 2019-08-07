#include "libx.h"
#include "comm.h"

void mysql_timer_task(void)
{
    pthread_mutex_lock(&g_list_head_in_mysql.mutex);
    pthread_mutex_lock(&g_list_head_out_mysql.mutex);

    comm_mysql_insert();
    comm_mysql_del();

    pthread_mutex_unlock(&g_list_head_in_mysql.mutex);
    pthread_mutex_unlock(&g_list_head_out_mysql.mutex);
}
