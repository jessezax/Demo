#include "libx.h"
#include "comm.h"

//static u64 rule_increase_idx_start = 0;

bool need_escape(char ch)
{
    const char  need_escape_tab[] = {'*', '|', '.', '?', '+', '$', '^', '(', ')', '[', ']', '{', '}'};
    int         i;
    for(i=0; i<sizeof(need_escape_tab); i++)
    {
        if(need_escape_tab[i] == ch)
        {
            return true;
        }
    }
    return false;
}


bool generate_comm_match_from_mask(char *src, u8 *mask, u32 len, char **pos, char **tail, u8 encode_method, int offset)
{
    int count   = 0;
    int i       = 0;
    int b_len   = 0;
    bool s_flag = true;

    if(!encode_method)
    {
        while(i < len)
        {
            if(0xff == mask[i])
            {
                if(count)
                {
                    if(offset > 0)
                    {
                        if(s_flag)
                        {
                            RULE_PUT(*pos, *tail, resolve_error, ".{%d}", offset + count);
                        }
                        else
                        {
                            RULE_PUT(*pos, *tail, resolve_error, ".{%d}", count);
                        }
                    }
                    else if(!offset)
                    {
                        RULE_PUT(*pos, *tail, resolve_error, ".{%d}", count);
                    }
                    else if(!s_flag)
                    {
                        RULE_PUT(*pos, *tail, resolve_error, ".{%d}", count);
                    }
                    count = 0;
                }
                #if 1
                if(need_escape(src[i]))
                {
                    RULE_PUT(*pos, *tail, resolve_error, "\\");
                }
                #endif
                RULE_PUT(*pos, *tail, resolve_error, "%c", src[i]);
                s_flag = false;
            }
            else if(0x0 == mask[i])
            {
                count++;
            }
            else
            {
                return false;
            }
            i++;
        }
    }
    else
    {
        b_len = len / 2;
        while(i < b_len)
        {
            if(0xff == mask[i*2] && 0xff == mask[i*2 + 1])
            {
                if(count)
                {
                    if(offset > 0)
                    {
                        if(s_flag)
                        {
                            RULE_PUT(*pos, *tail, resolve_error, ".{%d}", offset + count);
                        }
                        else
                        {
                            RULE_PUT(*pos, *tail, resolve_error, ".{%d}", count);
                        }
                    }
                    else if(!offset)
                    {
                        RULE_PUT(*pos, *tail, resolve_error, ".{%d}", count);
                    }
                    else if(!s_flag)
                    {
                        RULE_PUT(*pos, *tail, resolve_error, ".{%d}", count);
                    }
                    count = 0;
                }
                RULE_PUT(*pos, *tail, resolve_error, "\\x%c%c", src[i*2], src[i*2+1]);
                s_flag = false;
            }
            else if(0xff == mask[i*2] && 0x00 == mask[i*2 + 1])
            {
                if(count)
                {
                    if(offset > 0)
                    {
                        if(s_flag)
                        {
                            RULE_PUT(*pos, *tail, resolve_error, ".{%d}", offset + count);
                        }
                        else
                        {
                            RULE_PUT(*pos, *tail, resolve_error, ".{%d}", count);
                        }
                    }
                    else if(!offset)
                    {
                        RULE_PUT(*pos, *tail, resolve_error, ".{%d}", count);
                    }
                    else if(!s_flag)
                    {
                        RULE_PUT(*pos, *tail, resolve_error, ".{%d}", count);
                    }
                    count = 0;
                }
                RULE_PUT(*pos, *tail, resolve_error, "\\x%c0", src[i*2]);
                s_flag = false;
            }
            else if(0x0 == mask[i*2] && 0x0 == mask[i*2 + 1])
            {
                count++;
            }
            else
            {
                return false;
            }
            i++;
        }
    }

    return true;
resolve_error:
    return false;
}

bool comm_rule_generate(t_rule_generate_one *rule, t_rule_file *rule_file, e_action_type op, e_file_type file_type)
{
    char    action[1024]    = {0};
    int     action_len      = 0;
    bool    retval          = true;
    char    *act[8]         = {"drop","fwd"};
    char    *oft[8]         = {"both", "tcp", "udp"};       ///< 特征码需要
    char    *opr[8]         = {"add", "del"};

    if(rule->action.app_id)
    {
        action_len = snprintf(action, 1024, "mirror,bdrct=both,mtype=flow,htype=broadcast,s=%s,e=2999-12-31 23:59:59,"
                                    "g=%u,act=%s,pst=%u,hst=%u,rid=%u,isp=%u,uid=%u,opr=%s,mlt=%u,aid=%lu",
                                    rule->action.start_time,
                                    rule->action.port_group,
                                    act[rule->action.act],
                                    rule->action.save,
                                    rule->action.hst,
                                    rule->action.rule_id,
                                    rule->action.isp,
                                    rule->action.user_id,
                                    opr[op],
                                    rule->action.single,
                                    rule->action.app_id);
    }
    else
    {
        action_len = snprintf(action, 1024, "mirror,bdrct=both,mtype=flow,htype=broadcast,s=%s,e=2999-12-31 23:59:59,"
                                    "g=%u,act=%s,pst=%u,hst=%u,rid=%u,isp=%u,uid=%u,opr=%s,mlt=%u",
                                    rule->action.start_time,
                                    rule->action.port_group,
                                    act[rule->action.act],
                                    rule->action.save,
                                    rule->action.hst,
                                    rule->action.rule_id,
                                    rule->action.isp,
                                    rule->action.user_id,
                                    opr[op],
                                    rule->action.single);
    }

    if(rule->action.oft)
    {
        action_len += snprintf(action + action_len ,1024, ",l4pro=%s", oft[rule->action.oft - 1]);
    }
    else
    {
        action_len += snprintf(action + action_len ,1024, ",l4pro=both");
    }

    if(rule->action.pro[0])
    {
        action_len += snprintf(action + action_len ,1024, ",pne=%s", rule->action.pro);
    }

    retval  = rule_file_append(rule_file, NULL, rule->match, action, file_type);

    return retval;
}

void rule_file_ctx_destroy(t_rule_file_ctx *ctx, e_file_type file_type)
{
    if(ctx->tuple_file_full)
    {
        rule_file_free(ctx->tuple_file_full, file_full);
        ctx->tuple_file_full = NULL;
    }
    if(ctx->tuple_file_increase)
    {
        rule_file_free(ctx->tuple_file_increase, file_type);
        ctx->tuple_file_increase = NULL;
    }
    if(ctx->singel_file_full)
    {
        rule_file_free(ctx->singel_file_full, file_full);
        ctx->singel_file_full = NULL;
    }
    if(ctx->singel_file_increase)
    {
        rule_file_free(ctx->singel_file_increase, file_type);
        ctx->singel_file_increase = NULL;
    }
    if(ctx->comm_file_full)
    {
        rule_file_free(ctx->comm_file_full, file_full);
        ctx->comm_file_full = NULL;
    }
    if(ctx->comm_file_increase)
    {
        rule_file_free(ctx->comm_file_increase, file_type);
        ctx->comm_file_increase = NULL;
    }
    if(ctx->flow_file_full)
    {
        rule_file_free(ctx->flow_file_full, file_full);
        ctx->flow_file_full = NULL;
    }
    if(ctx->flow_file_increase)
    {
        rule_file_free(ctx->flow_file_increase, file_type);
        ctx->flow_file_increase = NULL;
    }
}

bool rule_file_ctx_init(t_rule_file_ctx *ctx, e_file_type file_type)
{
    if(!ctx)
    {
        return false;
    }
    memset(ctx, 0x0, sizeof(t_rule_file_ctx));

    if(file_full == file_type)
    {
        ctx->tuple_file_full = rule_file_alloc(rule_type_tuple_full, file_full);
        if(!ctx->tuple_file_full)
        {
            goto error;
        }
        ctx->singel_file_full = rule_file_alloc(rule_type_single_full, file_full);
        if(!ctx->singel_file_full)
        {
            goto error;
        }
        ctx->comm_file_full = rule_file_alloc(rule_type_comm_full, file_full);
        if(!ctx->comm_file_full)
        {
            goto error;
        }
        ctx->flow_file_full = rule_file_alloc(rule_type_flow_full, file_full);
        if(!ctx->flow_file_full)
        {
            goto error;
        }
        ctx->tuple_file_increase = rule_file_alloc(rule_type_tuple_increase, file_full);
        if(!ctx->tuple_file_increase)
        {
            goto error;
        }
        ctx->singel_file_increase = rule_file_alloc(rule_type_single_increase, file_full);
        if(!ctx->singel_file_increase)
        {
            goto error;
        }
        ctx->comm_file_increase = rule_file_alloc(rule_type_comm_increase, file_full);
        if(!ctx->comm_file_increase)
        {
            goto error;
        }
        ctx->flow_file_increase = rule_file_alloc(rule_type_flow_increase, file_full);
        if(!ctx->flow_file_increase)
        {
            goto error;
        }
    }
    else
    {
        ctx->tuple_file_increase = rule_file_alloc(rule_type_tuple_increase, file_inc);
        if(!ctx->tuple_file_increase)
        {
            goto error;
        }
        ctx->singel_file_increase = rule_file_alloc(rule_type_single_increase, file_inc);
        if(!ctx->singel_file_increase)
        {
            goto error;
        }
        ctx->comm_file_increase = rule_file_alloc(rule_type_comm_increase, file_inc);
        if(!ctx->comm_file_increase)
        {
            goto error;
        }
        ctx->flow_file_increase = rule_file_alloc(rule_type_flow_increase, file_inc);
        if(!ctx->flow_file_increase)
        {
            goto error;
        }
    }
    return true;
error:
    rule_file_ctx_destroy(ctx, file_full);
    return false;
}

bool file_ctx_complete(t_rule_file_ctx *ctx, e_file_type type)
{
    if(file_full == type)
    {
        rule_file_complete(ctx->comm_file_full);
        rule_file_complete(ctx->tuple_file_full);
        rule_file_complete(ctx->singel_file_full);
        rule_file_complete(ctx->flow_file_full);
    }

    rule_file_complete(ctx->singel_file_increase);
    rule_file_complete(ctx->comm_file_increase);
    rule_file_complete(ctx->tuple_file_increase);
    rule_file_complete(ctx->flow_file_increase);

    return true;
}

bool file_ctx_update(t_rule_file_ctx *ctx, e_file_type file_type)
{
    if(file_full == file_type)          ///< 如果是全量更新, 则执行下面步骤
    {
        rule_version_update(ctx->comm_file_full, file_type);
        rule_version_update(ctx->singel_file_full, file_type);
        rule_version_update(ctx->tuple_file_full, file_type);
        rule_version_update(ctx->flow_file_full, file_type);
    }

    rule_version_update(ctx->comm_file_increase, file_type);
    rule_version_update(ctx->singel_file_increase, file_type);
    rule_version_update(ctx->tuple_file_increase, file_type);
    rule_version_update(ctx->flow_file_increase, file_type);

    return true;
}

static void __rule_generate(t_rule_file_ctx *ctx, t_handle_msg_info *msg)
{
    switch (msg->sub_type)
    {
        case comm_sub_msg_account:
        {
            __account_rule_generate(ctx, msg, action_add, file_full, rule_file_type_comm);
            break;
        }
        default:
        {
            return;
        }
    }
}

void rule_generate()
{
    int idx;
    t_rule_file_ctx     ctx;
    t_handle_msg_info   *handle_msg = NULL;
    g_rule_idx = g_rule_start_idx;

    pthread_mutex_lock(&g_list_head_in_mysql.mutex);
    pthread_mutex_lock(&g_list_head_out_mysql.mutex);

    rule_file_ctx_init(&ctx, file_full);

    for (idx = 0; idx < HASH_TABLE_SIZE; idx ++)
    {
        xlist_for_each_entry(handle_msg, &g_list_head_in_mysql.list_head[idx], list_head)
        {
            __rule_generate(&ctx, handle_msg);
        }

        xlist_for_each_entry(handle_msg, &g_list_head_out_mysql.list_head[idx], list_head)
        {
            __rule_generate(&ctx, handle_msg);
        }
    }

    file_ctx_complete(&ctx, file_full);
    file_ctx_update(&ctx, file_full);
    rule_file_ctx_destroy(&ctx, file_full);

    pthread_mutex_unlock(&g_list_head_in_mysql.mutex);
    pthread_mutex_unlock(&g_list_head_out_mysql.mutex);

    curr_full_rule_num = 0;
    curr_increase_rule_num = 0;

    return;
}

u32 success = 0;

/**
 * @brief                   策略新增规则生成
 * @param[in]  conn         TCP连接信息
 * @param[in]  type         策略消息类型
 * @param[in]  op           策略执行动作
 * @param[in]  handle_msg   策略具体消息内容
 * @param[in]  type         策略更新规则类型, 是pkt.payload还是flow.payload
 * @param[in]  arg
 * @return                  void
 */
void rule_generate_for_inc( t_connection * conn,
                            e_comm_msg_type msg_type,
                            e_action_type op,
                            t_handle_msg_info *handle_msg,
                            e_rule_file_type type,
                            void *arg)
{
    u64                 clock;
    t_rule_file_ctx     ctx;

    rule_file_ctx_init(&ctx, file_inc);
    clock = clock_monotonic();

    switch (msg_type)
    {
        case comm_msg_account_req:
        {
            __account_rule_generate(&ctx, handle_msg, op, file_inc, rule_file_type_comm);
            break;
        }
        default:
        {
            return;
        }
    }

    file_ctx_complete(&ctx, file_inc);
    if((curr_increase_rule_num >= g_max_increase_num) || ((clock - conn->last_time >= g_increase_freq) && curr_increase_rule_num))
    {
        file_ctx_update(&ctx, file_inc);
        conn->last_time = clock;
        curr_increase_rule_num = file_inc;
    }

    rule_file_ctx_destroy(&ctx, file_inc);

    return;
}


