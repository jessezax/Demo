#include "libx.h"
#include "comm.h"

static bool generate_account_match_from_mask(char *src, u8 *mask, u32 len, char **pos, char **tail, u8 type)
{
    int count       = 0;
    int i           = 0;

    if(0 == type)
    {
        while(i < len)
        {
            if(mask[i] == 0xff)
            {
                if(count)
                {
                    RULE_PUT(*pos, *tail, resolve_error, "[ -~]{%d}", count);
                }
                if(need_escape(src[i]))
                {
                    RULE_PUT(*pos, *tail, resolve_error, "\\");
                }
                RULE_PUT(*pos, *tail, resolve_error, "\%c", src[i]);
                count = 0;
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
        while(i < 8)
        {
            if(src[2*i] && src[2*i+1])
            {
                if(0xff == mask[2*i] && 0xff == mask[2*i+1])
                {
                    if(count)
                    {
                        RULE_PUT(*pos, *tail, resolve_error, ".{%d}", count);
                    }
                    RULE_PUT(*pos, *tail, resolve_error, "\\x\%c\%c", src[2*i+1], src[2*i]);
                    count = 0;
                }
                else if(0x0 == mask[2*i] && 0x0 == mask[2*i+1])
                {
                    count++;
                }
                else
                {
                    goto error;
                }
            }
            else if(src[2*i])
            {
                if(0xff == mask[2*i] && 0xff == mask[2*i+1])
                {
                    if(count)
                    {
                        RULE_PUT(*pos, *tail, resolve_error, ".{%d}", count);
                    }
                    RULE_PUT(*pos, *tail, resolve_error, "\\xf\%c", src[2*i]);
                    count = 0;
                }
                else if(0x0 == mask[2*i] && 0x0 == mask[2*i+1])
                {
                    count++;
                }
                else
                {
                    goto error;
                }
            }
            else
            {
                if(0xff == mask[2*i] && 0xff == mask[2*i+1])
                {
                    if(count)
                    {
                        RULE_PUT(*pos, *tail, resolve_error, ".{%d}", count);
                    }
                    RULE_PUT(*pos, *tail, resolve_error, "\\xff");
                    count = 0;
                }
                else if(0x0 == mask[2*i] && 0x0 == mask[2*i+1])
                {
                    count++;
                }
                else
                {
                    goto error;
                }
            }
            i++;
        }
    }
    return true;
error:
resolve_error:
    return false;
}

bool account_match_generate(    char *match_pos,
                                char *match_tail,
                                t_account_info *info,
                                bool start_flag,
                                bool not_flag,
                                e_rule_file_type type)
{
    char    *accoount_type[8]           = {"acc", "ims", "msi", "ime"};
    char    account[1024]               = {0};
    char    *account_pos                = account;
    char    *account_tail               = account + 1024;

    if(!generate_account_match_from_mask(info->account, info->account_mask, 64, &account_pos, &account_tail, info->account_type))
    {
        return false;
    }

    if(!start_flag)
    {
        RULE_PUT(match_pos, match_tail, resolve_error, "&&");
    }
    if(not_flag)
    {
        RULE_PUT(match_pos, match_tail, resolve_error, "!");
    }
    RULE_PUT(match_pos, match_tail, resolve_error, "pkt.desc~\"%s_%s\"", accoount_type[info->account_type], account);

    return true;

resolve_error:
    return false;
}

void account_rule_free(t_rule_generate_one *rule)
{
    x_free(rule);
}

t_rule_generate_one *account_rule_alloc(    t_action_fields *action,
                                            t_account_info *info,
                                            e_rule_file_type type)
{
    t_rule_generate_one *rule;
    char                match[1024]     = {0};          ///< 负责存储入库策略match部分内容
    char                *match_pos      = match;
    char                *match_tail     = match + 1024;
    bool                start_flag      = true;
    bool                not_flag        = false;

    rule = x_zalloc(sizeof(t_rule_generate_one) + 1024);
    if(!rule)
    {
        return NULL;
    }

    memcpy(&rule->action, action, sizeof(t_action_fields));
    not_flag = (action->rule_type & 0x80) >> 7;

    if(!account_match_generate(match_pos, match_tail, info, start_flag, not_flag, type))
    {
        account_rule_free(rule);
        return NULL;
    }

    snprintf(rule->match, strlen(match) + 1, "%s", match);

    return rule;
}

bool __account_rule_generate(   t_rule_file_ctx *ctx,
                                t_handle_msg_info *msg,
                                e_action_type op,
                                e_file_type file_type,
                                e_rule_file_type type)
{
    char                start_time[32]      = {0};
    char                action_buf[1024]    = {0};
    t_action_fields     *action             = (t_action_fields *)action_buf;
    t_action_add        *add_info           = (t_action_add *)msg->data;
    t_account_info      *acc_info           = (t_account_info *)(msg->data + sizeof(t_action_add));

    t_rule_generate_one *rule       = NULL;

    time2str(add_info->timestamp, start_time, 32);

    action->user_id     = add_info->user_id;
    action->port_group  = add_info->port_group;
    action->act         = add_info->act;
    action->save        = add_info->save;
    action->hst         = add_info->hst;
    action->isp         = add_info->isp;
    action->rule_id     = add_info->rule_id;
    action->rule_type   = add_info->type[0];

    memcpy(action->start_time, start_time, strlen(start_time));

    rule = account_rule_alloc(action, acc_info, type);
    if(!rule)
    {
        printf("rule_id is %u\n", action->rule_id);
        return false;
    }
    if(file_full == file_type)
    {
        comm_rule_generate(rule, ctx->comm_file_full, op, file_type);
    }
    else
    {
        comm_rule_generate(rule, ctx->comm_file_increase, op, file_type);
    }

    account_rule_free(rule);

    return true;
}

