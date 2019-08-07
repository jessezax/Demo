#include "libx.h"

//在cur_node节点上按path路径查找节点，path可以直接为属性名
static const char *x_conf_get_attr_val(mxml_node_t *cur_node, const char *path)
{
    char path_local[1024];

    assert(cur_node);
    assert(path);

    snprintf(path_local, 1024, "%s", path);
    if (path_local[strlen(path_local) - 1] == '/')
    {
        path_local[strlen(path_local) - 1] = 0;
    }

    char *attr_name = rindex(path_local, '/');
    if (attr_name) {
        *attr_name = '\0';
        attr_name += 1;

        mxml_node_t *curnode = mxmlFindPath(cur_node, path_local);
        if (curnode && curnode->type == MXML_ELEMENT)
        {
            const char *value = mxmlElementGetAttr(curnode, attr_name);
            return value;
        }
    }
    else
    {
        const char *value = mxmlElementGetAttr(cur_node, path_local);
        return value;
    }
    return NULL;
}

const char *__x_conf_get_value(void *dstaddr, XCONF_VTYPE dtype, int dlen, const char *path, mxml_node_t *node)
{
    const char *value = x_conf_get_attr_val(node, path);
    if (value == NULL)
    {
        return NULL;
    }

    if (dstaddr && dtype != XCONF_NONE)
    {
        switch (dtype)
        {
            case XCONF_S1:
                *(s8 *)dstaddr = (s8)atoi(value);
                break;
            case XCONF_S2:
                *(s16 *)dstaddr = (s16)atoi(value);
                break;
            case XCONF_S4:
                *(s32 *)dstaddr = (s32)atoi(value);
                break;
            case XCONF_S8:
                *(s64 *)dstaddr = (s64)atoll(value);
                break;
            case XCONF_U1:
                *(u8 *)dstaddr = (u8)atoi(value);
                break;
            case XCONF_U2:
                *(u16 *)dstaddr = (u16)atoi(value);
                break;
            case XCONF_U4:
                *(u32 *)dstaddr = (u32)atoi(value);
                break;
            case XCONF_U8:
                *(u64 *)dstaddr = (u64)atoll(value);
                break;
            case XCONF_DOUBLE:
                *(double *)dstaddr = (double)atof(value);
                break;
            case XCONF_STRING:
                snprintf((char *)dstaddr, dlen, "%s", value);
                break;
            default:
                return NULL;
        }
    }
    return value;
}

const char * x_conf_get_value(p_conf_ctx pctx,void *dstaddr, XCONF_VTYPE dtype, int dlen, const char *path)
{
    t_conf_ctx * ctx = pctx;
    return __x_conf_get_value(dstaddr, dtype, dlen, path, ctx->conf_xml_tree);
}

//返回初始化次数
int x_conf_get_value_arr(p_conf_ctx   pctx,
                            void       *dstaddr,
                            XCONF_VTYPE dtype,
                            int         dlen,
                            const char *list_path,
                            const char *path,
                            int         jump_size)
{
    int i = 0;
    t_conf_ctx * ctx = pctx;
    mxml_node_t *cur_node = mxmlFindPath(ctx->conf_xml_tree, list_path);
    if (cur_node == NULL)
    {
        return 0;
    }

    //确保定位到list节点
    cur_node = cur_node->type != MXML_ELEMENT ? cur_node->parent : cur_node;
    if (cur_node->type != MXML_ELEMENT)
    {
        fprintf(stderr, "find list path error:%s\n", list_path);
        return 0;
    }

    //遍历兄弟节点
    while (cur_node)
    {
        if (strncmp(cur_node->value.element.name, "!--", 3) == 0)
        {
            cur_node = mxmlGetNextSibling(cur_node);
            cur_node = cur_node->type != MXML_ELEMENT ? cur_node->next : cur_node;
            continue;
        }

        void *new_addr = (u8 *)dstaddr + i * jump_size;
        __x_conf_get_value(dstaddr ? new_addr : NULL, dtype, dlen, path, cur_node);
        i++;

        cur_node = mxmlGetNextSibling(cur_node);
        cur_node = cur_node->type != MXML_ELEMENT ? cur_node->next : cur_node;
    }

    return i;
}

static inline
mxml_node_t * x_conf_file_load(const char *conf_path)
{
    FILE        * file;
    mxml_node_t * conf_xml_tree;

    file = fopen(conf_path, "r");
    if (file == NULL) {
        return NULL;
    }

    conf_xml_tree = mxmlLoadFile(NULL, file, MXML_NO_CALLBACK);

    fclose(file);
    return conf_xml_tree;
}

p_conf_ctx x_conf_load(const char *conf_path)
{
    t_conf_ctx * ctx;
    ctx = x_zalloc(sizeof(t_conf_ctx));
    if (!ctx) {
        return NULL;
    }
    if (!__string_dup(conf_path, strlen(conf_path), &ctx->file_path)) {
        goto string_dup_error;
    }
    ctx->conf_xml_tree = x_conf_file_load(conf_path);
    if (!ctx->conf_xml_tree) {
        goto conf_file_load_error;
    }

    return ctx;
conf_file_load_error:
    string_free(&ctx->file_path);
string_dup_error:
    x_free(ctx);
    return NULL;
}


void x_conf_free(p_conf_ctx pctx)
{
    t_conf_ctx * ctx = pctx;
    if (ctx->conf_xml_tree) {
        mxmlDelete(ctx->conf_xml_tree);
        ctx->conf_xml_tree = NULL;
    }
    string_free(&ctx->file_path);
    x_free(ctx);
}

