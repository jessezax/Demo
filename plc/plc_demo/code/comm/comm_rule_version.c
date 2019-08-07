#include "libx.h"
#include "comm.h"

static char             g_publish_base_path[256]        = "/var/www/html";
static t_rule_version   g_publish_rules[rule_type_num];
static int              md5check                        = 0;

static bool __comm_rule_version_reload(const char * name,e_rule_type type,e_comm_plc_type plc_type)
{
    char buf[256];
    int  len;
    t_rule_version * rule_version;
    rule_version = &g_publish_rules[type];

    snprintf(rule_version->rule_name,64,"%s",name);
    snprintf(rule_version->rule_filename,64,"%s.conf",name);
    snprintf(rule_version->version_path,256,"%s/%s_version",g_publish_base_path,name);
    snprintf(rule_version->rule_path,256,"%s/%s",g_publish_base_path,rule_version->rule_filename);

    len = file_get_contents(rule_version->version_path, 0,buf, 256);
    if (len > 0) {
        if (!atou32(buf,len,&rule_version->version)) {
            return false;
        }
    } else {
        rule_version->version = 0;
    }
    rule_version->type = type;
    rule_version->plc_type = plc_type;
    rule_version->tmp_version = rule_version->version + 10000;
    return true;
}

static bool comm_rule_version_reload(void)
{
    if (!x_conf_get_value(g_conf_ctx,(void*)g_publish_base_path,XCONF_STRING,256,"root/core/rule_version/base_path")) {
        //return false;
    }

    plc_mkdirs(g_publish_base_path);

    if (!__comm_rule_version_reload("comm_increase", rule_type_comm_increase,lte_incremental_update)) {
        return false;
    }
    if (!__comm_rule_version_reload("comm_full", rule_type_comm_full,lte_full_update)) {
        return false;
    }
    if (!__comm_rule_version_reload("tuple_increase", rule_type_tuple_increase,lte_tuple_incremental_update)) {
        return false;
    }
    if (!__comm_rule_version_reload("tuple_full", rule_type_tuple_full,lte_tuple_full_update)) {
        return false;
    }
    if (!__comm_rule_version_reload("cmb_increase", rule_type_single_increase,lte_cmb_incremental_update)) {
        return false;
    }
    if (!__comm_rule_version_reload("cmb_full", rule_type_single_full,lte_cmb_full_update)) {
        return false;
    }
    if (!__comm_rule_version_reload("flow_increase", rule_type_flow_increase,lte_flow_incremental_update)) {
        return false;
    }
    if (!__comm_rule_version_reload("flow_full", rule_type_flow_full,lte_flow_full_update)) {
        return false;
    }

    return true;
}

u32 rule_version(e_rule_type type)
{
    return g_publish_rules[type].version;
}

u32 rule_version_new(e_rule_type type)
{
    return g_publish_rules[type].tmp_version++;
}

u32 get_file_size(const char* file) {
    struct stat tbuf;
    stat(file, &tbuf);
    return tbuf.st_size;
}

static unsigned char *md5_cal(char *file)
{
    unsigned char   buff[64]        = {0};
    unsigned char   *md5            = buff;
    char            buffer[1024]    = {0};
    FILE            *fp             = NULL;
    MD5_CTX ctx;
    int             len             = 0;

    fp = fopen(file, "rb");
    if(!fp)
    {
        return NULL;
    }

    MD5_Init(&ctx);
    while((len = fread(buffer, 1, 1024, fp)) > 0)
    {
        // 消除规则前面ID对MD5的影响
        MD5_Update(&ctx, buffer + 8, len - 8);
        bzero(buffer, 1024);
    }
    MD5_Final(md5, &ctx);
    fclose(fp);

    return md5;
}

bool rule_version_update(t_rule_file * file, e_file_type file_type)
{
    char            buf[256]            = {0};
    char            rule_bak[256]       = {0};
    char            version_bak[256]    = {0};
    char            path[512]           = {0};
    char            md5_buf[256]        = {0};
    char            *md5                = md5_buf;
    char            old_md5[32]         = {0};
    char            new_md5[32]         = {0};
    t_rule_version  * rule_version;
    rule_version = &g_publish_rules[file->type];

    if (file_exist(rule_version->rule_path)) {
        md5 = (char *)md5_cal(rule_version->rule_path);
        memcpy(old_md5, md5, 16);
        md5 = (char *)md5_cal(file->file_path);
        memcpy(new_md5, md5, 16);

        if(!md5check || strncmp(old_md5, new_md5, 16))
        {
            g_publish_rules[file->type].version++;
        }
        else
        {
            return true;
        }
        snprintf(rule_bak,256,"%s.prev",rule_version->rule_path);
        if (file_mv(rule_version->rule_path, rule_bak) < 0) {
            goto rollback1;
        }
    }
    else
    {
        if(!get_file_size(file->file_path))
        {
            return true;
        }
        else
        {
            rule_version->version = 1;
        }
    }

    if(file_full == file_type)
    {
        if (file_mv(file->file_path, rule_version->rule_path) < 0) {
            goto rollback2;
        }
    }
    else
    {
        if (file_copy(file->file_path, rule_version->rule_path) < 0) {
            goto rollback2;
        }
    }

    if (file_exist(rule_version->version_path)) {
        snprintf(version_bak,256,"%s.prev",rule_version->version_path);
        if (file_mv(rule_version->version_path, version_bak) < 0) {
            goto rollback3;
        }
    }
    snprintf(buf,256,"%u",rule_version->version);

    if (file_put_contents(rule_version->version_path, 0,buf, strlen(buf)) < 0) {
        goto rollback4;
    }

    snprintf(path,512,"%s/%s",g_publish_base_path,rule_version->rule_filename);

    comm_plc_notify(rule_version->plc_type, rule_version->version, comm_data_type_data, path, strlen(path));

    return true;
rollback4:
    file_mv(version_bak,rule_version->version_path);
rollback3:
    remove(rule_version->rule_path);
    file_mv(rule_bak,rule_version->rule_path);
rollback2:
rollback1:
    return false;
}

static int comm_rule_version_init(t_xmodule * xmod)
{
    if (!x_conf_get_value(g_conf_ctx,(void*)&md5check,XCONF_U4,64,"root/core/file_md5/md5check")) {
    }

    if (!comm_rule_version_reload()) {
        return -1;
    }
    return 0;
}

static int comm_rule_version_uninit(t_xmodule * xmod)
{
    return 0;
}

static int comm_rule_version_reconf(t_xmodule * xmod)
{
    return 0;
}

static int comm_rule_version_ctrl(t_xmodule * xmod,unsigned int ctrl,void * arg,unsigned int arg_size)
{
    return 0;
}

static  t_xmodule comm_xmod_rule_version = {
    .name = "comm rule version",
    .idx = XMOD_RULE_VERSION_IDX,
    .init = comm_rule_version_init,
    .uninit = comm_rule_version_uninit,
    .reconf = comm_rule_version_reconf,
    .ctrl = comm_rule_version_ctrl,
    .ctx = NULL
};

x_module_register(&comm_xmod_rule_version,XMOD_RULE_VERSION_IDX)
