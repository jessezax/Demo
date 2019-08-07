#include "libx.h"
#include "comm.h"

t_users_info    users_info;

static bool users_info_load(void)
{
    int     num, i;
    char    passwd[MAX_USER_NUM][MAX_PASSWD_LEN] = {{0}};

    num = x_conf_get_value_arr(g_conf_ctx,
                                            (void *)&users_info.user_info[0].user_id,
                                            XCONF_U1, 0,
                                            "root/user_info_list/user",
                                            "user_id",
                                            sizeof(t_user_info));

    num = x_conf_get_value_arr(g_conf_ctx,
                                            (void *)&users_info.user_info[0].username,
                                            XCONF_STRING, MAX_USERNAME_LEN,
                                            "root/user_info_list/user",
                                            "username",
                                            sizeof(t_user_info));
    num = x_conf_get_value_arr(g_conf_ctx,
                                            (void *)&passwd[0],
                                            XCONF_STRING, MAX_PASSWD_LEN,
                                            "root/user_info_list/user",
                                            "passwd",
                                            sizeof(t_user_info));
    users_info.num = num;

    for(i=0; i<num; i++)
    {
        MD5_CTX ctx;
        MD5_Init(&ctx);
        MD5_Update(&ctx, passwd[i], strlen(passwd[i]));
        MD5_Final((unsigned char *)users_info.user_info[i].passwd, &ctx);
    }

    return true;
}

void users_info_init(void)
{
    memset((void *)&users_info, 0x0, sizeof(t_users_info));     ///< 初始化
    users_info_load();                                  ///< 配置中加载
}
