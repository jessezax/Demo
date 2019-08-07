#ifndef __PM_MSG_USER_H__
#define __PM_MSG_USER_H__

#define     MAX_USERNAME_LEN    64
#define     MAX_PASSWD_LEN      64
#define     MAX_USER_NUM        256

typedef struct
{
    u8              user_id;                            ///< 用户ID
    char            username[MAX_USERNAME_LEN];         ///< 用户名
    char            passwd[MAX_PASSWD_LEN];             ///< 密码
}t_user_info;

typedef struct
{
    int             num;
    t_user_info     user_info[MAX_USER_NUM];
}t_users_info;

extern t_users_info users_info;

void users_info_init(void);

#endif
