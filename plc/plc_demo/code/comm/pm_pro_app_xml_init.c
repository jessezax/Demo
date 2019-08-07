#include "libx.h"
#include "comm.h"

#define MAX_LEN 1024
char    g_appid_file[64]            = "appid.xml";
char    g_url_list_path[64]         = "appcfg/hosturl";
char    g_uri_list_path[64]         = "appcfg/urlinfo";
char    g_host_list_path[64]        = "appcfg/hostinfo";
char    g_useragent_list_path[64]   = "appcfg/useragentinfo";

t_hash_list_heads_for_pro_app   g_list_head_for_pro_app;

static bool url_info_load(t_conf_ctx *ctx)
{
    int     host_len        = 0;
    int     uri_len         = 0;
    char    host_buf[1024]  = {0};
    char    uri_buf[1024]   = {0};
    char    id_buf[1024]    = {0};
    int     i, id_num;
    u64     id[256]         = {0};

    mxml_node_t *cur_node, *s_node;
    //获取hosturl信息
    cur_node = mxmlFindPath(ctx->conf_xml_tree, g_url_list_path);

    if (!cur_node)
    {
        return false;
    }

    //确保定位到list节点
    cur_node = cur_node->type != MXML_ELEMENT ? cur_node->parent : cur_node;
    if (cur_node->type != MXML_ELEMENT)
    {
        fprintf(stderr, "find list path error:%s\n", g_url_list_path);
        return false;
    }

    for (;cur_node != NULL;cur_node = mxmlFindElement(cur_node, ctx->conf_xml_tree,
                                "hosturl",
                                NULL, NULL,
                                MXML_NO_DESCEND))
    {
        bzero(host_buf, 1024);
        __x_conf_get_value(host_buf, XCONF_STRING, 1024, "host", cur_node);
        //printf("host_buf = %s\n", host_buf);

        s_node = mxmlFindElement(cur_node, ctx->conf_xml_tree,
                                    "url",
                                    NULL, NULL,
                                    MXML_DESCEND_FIRST);
        for (;s_node != NULL;s_node = mxmlFindElement(s_node, ctx->conf_xml_tree,
                                    "url",
                                    NULL, NULL,
                                    MXML_DESCEND_FIRST))
        {
            bzero(uri_buf, 1024);
            __x_conf_get_value(uri_buf, XCONF_STRING, 1024, "value", s_node);
            //printf("uri_buf = %s\n", uri_buf);
            bzero(id_buf, 1024);
            __x_conf_get_value(id_buf, XCONF_STRING, 1024, "id", s_node);

            for(i = 0; i < 256; i++)
            {
                id[i] = 0;
            }
            id_num = id_str_to_int(id_buf, id);

            for(i=0; i<id_num; i++)
            {
                t_pro_app_type_info *info = NULL;

                if (uri_buf[0])
                {
                    uri_len = strlen(uri_buf);
                }
                else
                {
                    uri_len = 0;
                }

                if (host_buf[0])
                {
                    host_len = strlen(host_buf);
                }
                else
                {
                    host_len = 0;
                }

                info = x_zalloc(sizeof(t_pro_app_type_info) + uri_len + host_len + 1);
                if (!info)
                {
                    return false;
                }

                info->app_id    =   id[i];
                info->type      =   pro_app_url;
                info->uri_len   =   uri_len;
                info->data_len  =   uri_len + host_len;

                memcpy(info->data, uri_buf, uri_len);
                memcpy(info->data + uri_len, host_buf, host_len);

                pm_pro_app_insert(info);
            }
        }
    }
    return true;
}

static bool host_info_load(t_conf_ctx *ctx)
{
    int     host_len        = 0;
    char    host_buf[1024]  = {0};
    char    id_buf[1024]    = {0};
    int     i, id_num;
    u64     id[256]         = {0};

    mxml_node_t *cur_node;
    //获取hosturl信息
    cur_node = mxmlFindPath(ctx->conf_xml_tree, g_host_list_path);

    if (!cur_node)
    {
        return false;
    }

    //确保定位到list节点
    cur_node = cur_node->type != MXML_ELEMENT ? cur_node->parent : cur_node;
    if (cur_node->type != MXML_ELEMENT)
    {
        fprintf(stderr, "find list path error:%s\n", g_host_list_path);
        return false;
    }

    //遍历兄弟节点
    while (cur_node)
    {
        if (strncmp(cur_node->value.element.name, "hostinfo", strlen("hostinfo")) != 0)
        {
            cur_node = mxmlGetNextSibling(cur_node);
            cur_node = cur_node->type != MXML_ELEMENT ? cur_node->next : cur_node;
            continue;
        }

        bzero(host_buf, 1024);
        __x_conf_get_value(host_buf, XCONF_STRING, 1024, "value", cur_node);
        bzero(id_buf, 1024);
        __x_conf_get_value(id_buf, XCONF_STRING, 1024, "id", cur_node);

        for (i = 0; i < 256; i++)
        {
            id[i] = 0;
        }
        id_num = id_str_to_int(id_buf, id);

        for (i=0; i<id_num; i++)
        {
            t_pro_app_type_info *info   = NULL;

            if (host_buf[0])
            {
                host_len = strlen(host_buf);
            }
            else
            {
                continue;
            }

            info = x_zalloc(sizeof(t_pro_app_type_info) + host_len + 1);
            if (!info)
            {
                return false;
            }

            info->app_id    = id[i];
            info->type      = pro_app_host;
            info->data_len  = host_len;
            memcpy(info->data, host_buf, host_len);

            pm_pro_app_insert(info);
        }

        cur_node = mxmlGetNextSibling(cur_node);
        cur_node = cur_node->type != MXML_ELEMENT ? cur_node->next : cur_node;
    }
    return true;
}

static bool uri_info_load(t_conf_ctx *ctx)
{
    int     uri_len         = 0;
    char    uri_buf[1024]   = {0};
    char    id_buf[1024]    = {0};
    int     i, id_num;
    u64     id[256]         = {0};

    mxml_node_t *cur_node;
    //获取hosturl信息
    cur_node = mxmlFindPath(ctx->conf_xml_tree, g_uri_list_path);

    if (!cur_node)
    {
        return false;
    }

    //确保定位到list节点
    cur_node = cur_node->type != MXML_ELEMENT ? cur_node->parent : cur_node;
    if (cur_node->type != MXML_ELEMENT)
    {
        fprintf(stderr, "find list path error:%s\n", g_uri_list_path);
        return false;
    }

    //遍历兄弟节点
    while (cur_node)
    {
        if (strncmp(cur_node->value.element.name, "urlinfo", strlen("urlinfo")) != 0)
        {
            cur_node = mxmlGetNextSibling(cur_node);
            cur_node = cur_node->type != MXML_ELEMENT ? cur_node->next : cur_node;
            continue;
        }

        bzero(uri_buf, 1024);
        __x_conf_get_value(uri_buf, XCONF_STRING, 1024, "value", cur_node);
        bzero(id_buf, 1024);
        __x_conf_get_value(id_buf, XCONF_STRING, 1024, "id", cur_node);

        for (i = 0; i < 256; i++)
        {
            id[i] = 0;
        }
        id_num = id_str_to_int(id_buf, id);

        for (i=0; i<id_num; i++)
        {
            t_pro_app_type_info *info   = NULL;

            if (uri_buf[0])
            {
                uri_len = strlen(uri_buf);
            }
            else
            {
                continue;
            }

            info = x_zalloc(sizeof(t_pro_app_type_info) + uri_len + 1);
            if (!info)
            {
                return false;
            }

            info->app_id    = id[i];
            info->type      = pro_app_uri;
            info->data_len  = uri_len;
            memcpy(info->data, uri_buf, uri_len);

            pm_pro_app_insert(info);
        }

        cur_node = mxmlGetNextSibling(cur_node);
        cur_node = cur_node->type != MXML_ELEMENT ? cur_node->next : cur_node;
    }
    return true;
}

static bool useragent_info_load(t_conf_ctx *ctx)
{
    int     user_agenet_len     = 0;
    char    useragent_buf[1024] = {0};
    char    id_buf[1024]        = {0};
    int     i, id_num;
    u64     id[256]             = {0};

    mxml_node_t *cur_node;
    //获取hosturl信息
    cur_node = mxmlFindPath(ctx->conf_xml_tree, g_useragent_list_path);

    if (!cur_node)
    {
        return false;
    }

    //确保定位到list节点
    cur_node = cur_node->type != MXML_ELEMENT ? cur_node->parent : cur_node;
    if (cur_node->type != MXML_ELEMENT)
    {
        fprintf(stderr, "find list path error:%s\n", g_uri_list_path);
        return false;
    }

    //遍历兄弟节点
    while (cur_node)
    {
        if (strncmp(cur_node->value.element.name, "useragentinfo", strlen("useragentinfo")) != 0)
        {
            cur_node = mxmlGetNextSibling(cur_node);
            cur_node = cur_node->type != MXML_ELEMENT ? cur_node->next : cur_node;
            continue;
        }

        bzero(useragent_buf, 1024);
        __x_conf_get_value(useragent_buf, XCONF_STRING, 1024, "value", cur_node);
        bzero(id_buf, 1024);
        __x_conf_get_value(id_buf, XCONF_STRING, 1024, "id", cur_node);

        for(i = 0; i < 256; i++)
        {
            id[i] = 0;
        }
        id_num = id_str_to_int(id_buf, id);

        for(i=0; i<id_num; i++)
        {
            t_pro_app_type_info *info   = NULL;

            if (useragent_buf[0])
            {
                user_agenet_len = strlen(useragent_buf);
            }
            else
            {
                continue;
            }

            info = x_zalloc(sizeof(t_pro_app_type_info) + user_agenet_len + 1);
            if (!info)
            {
                return false;
            }

            info->app_id    = id[i];
            info->type      = pro_app_user_agent;
            info->data_len  = user_agenet_len;
            memcpy(info->data, useragent_buf, user_agenet_len);

            pm_pro_app_insert(info);
        }

        cur_node = mxmlGetNextSibling(cur_node);
        cur_node = cur_node->type != MXML_ELEMENT ? cur_node->next : cur_node;
    }
    return true;
}

bool comm_appid_info_load(void)
{
    int     idx;

    for (idx = 0; idx < PRO_APP_HASH_TABLE_SIZE; idx++)
    {
        INIT_XLIST_HEAD(&g_list_head_for_pro_app.list_head[idx]);
    }

    x_conf_get_value(g_conf_ctx,(void*)g_appid_file,XCONF_STRING,64,"root/core/xml_file/value");

    t_conf_ctx *ctx = (t_conf_ctx *)x_conf_load(g_appid_file);
    if(!ctx)
    {
        printf("open appid.xml failed\n");
    }
    else
    {
        url_info_load(ctx);
        host_info_load(ctx);
        uri_info_load(ctx);
        useragent_info_load(ctx);
    }

    return true;
}

