#include "libx.h"
#include "comm.h"

static char g_tmp_file_base_path[256] = "./rule_tmp";

void plc_mkdirs(char *muldir)
{
    int i,len;
    char str[512];
    strncpy(str, muldir, 512);
    len=strlen(str);
    for(i = 0; i < len; i++)
    {
        if (str[i] == '/')
        {
            str[i] = '\0';
            if (access(str,0) != 0)
            {
                mkdir(str, 0777);
            }
            str[i]='/';
        }
    }
    if (len > 0 && access(str,0) != 0)
    {
        mkdir(str, 0777);
    }
    return;
}


static bool comm_rule_file_reload(void)
{
    if (!x_conf_get_value(g_conf_ctx,(void*)g_tmp_file_base_path,XCONF_STRING,256,"root/core/rule_file/base_path")) {
        //return false;
    }

    plc_mkdirs(g_tmp_file_base_path);

    return true;
}

int __rule_file_write(t_rule_file * file,void * data,int data_len)
{
    const char * pos;
    int total_len;
    int remain_len;
    int write_len;

    pos        = data;
    write_len  = 0;
    total_len  = 0;
    remain_len = data_len;

    while (remain_len > 0) {
        write_len = fwrite(pos,1,remain_len,file->fp);
        total_len += write_len;
        pos += write_len;

        if (write_len < remain_len) {
            if (ferror(file->fp)) {
                total_len = -1;
                break;
            }
        }
        remain_len -= write_len;
    }
    return total_len;
}

bool __rule_file_append(t_rule_file * file,const char * f,...)
{
    int retval;
    va_list var_list;
    va_start(var_list, f);
    retval = vfprintf(file->fp, f, var_list);
    //fflush(file->fp);
    va_end(var_list);
    if (retval < 0) {
        return false;
    }
    return true;
}

///< file_type, 0表示全量文件，1表示增量文件
bool rule_file_append(t_rule_file * file,char * user,char * match,char * action, e_file_type file_type)
{
    int user_len;
    int match_len;

    if (user) {
        user_len = strlen(user);
    } else {
        user_len = 0;
    }
    if (match) {
        match_len = strlen(match);
    } else {
        match_len = 0;
    }
    if (!user_len && !match_len) {
        return true;
    }

    __rule_file_append(file,"%07lu ", g_rule_idx++);

    if (user_len && match_len) {
        __rule_file_append(file,"%s&&%s ",user,match);
    } else if (user_len) {
        __rule_file_append(file,"%s ",user);
    } else if (match_len) {
        __rule_file_append(file,"%s ",match);
    }
    return __rule_file_append(file,"with action.do{%s}\n",action);
}

void rule_file_complete(t_rule_file * f)
{
    if (f->fp) {
        fflush(f->fp);
        fclose(f->fp);
        f->fp = NULL;
    }
}

void rule_file_free(t_rule_file * f, e_file_type file_type)
{
    rule_file_complete(f);
    if(file_full == file_type)
    {
        if (file_exist(f->file_path)) {
            remove(f->file_path);
        }
    }
    x_free(f);
}

t_rule_file * rule_file_alloc(e_rule_type type, e_file_type file_type)
{
    t_rule_file * file;

    file = x_zalloc(sizeof(t_rule_file));
    if (!file) {
        return NULL;
    }
    file->version = rule_version_new(type);

    snprintf(file->file_path,256,"%s/%u_%u",g_tmp_file_base_path,type,10000);
    #if 0
    if(file_flag)
    {
        snprintf(file->file_path,256,"%s/%u_%u",g_tmp_file_base_path,type,10000);
    }
    else
    {
        snprintf(file->file_path,256,"%s/%u_%u",g_tmp_file_base_path,type,file->version);
    }
    #endif
    file->type = type;
    //file->rule_idx = rule_idx;

    if(file_type)
    {
        file->fp = fopen(file->file_path,"w+");
    }
    else
    {
        file->fp = fopen(file->file_path,"a+");
    }
    if (!file->fp) {
        goto error;
    }
    return file;
error:
    rule_file_free(file, file_type);
    return NULL;
}

static int comm_rule_file_init(t_xmodule * xmod)
{
    if (!comm_rule_file_reload()) {
        return -1;
    }
    return 0;
}

static int comm_rule_file_uninit(t_xmodule * xmod)
{
    return 0;
}

static int comm_rule_file_reconf(t_xmodule * xmod)
{
    return 0;
}

static int comm_rule_file_ctrl(t_xmodule * xmod,unsigned int ctrl,void * arg,unsigned int arg_size)
{
    return 0;
}

static  t_xmodule comm_xmod_rule_file = {
    .name = "comm rule file",
    .idx = XMOD_RULE_FILE_IDX,
    .init = comm_rule_file_init,
    .uninit = comm_rule_file_uninit,
    .reconf = comm_rule_file_reconf,
    .ctrl = comm_rule_file_ctrl,
    .ctx = NULL
};

x_module_register(&comm_xmod_rule_file,XMOD_RULE_FILE_IDX)
