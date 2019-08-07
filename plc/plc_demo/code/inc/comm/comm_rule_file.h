#ifndef __comm_RULE_FILE_H__
#define __comm_RULE_FILE_H__

/* 文件类型 */
typedef enum {
    file_inc    = 0x00,     ///< 增量更新文件
    file_full   = 0x01,     ///< 全量更新文件
}e_file_type;

typedef enum {
    rule_type_none,
    rule_type_mirror,
    rule_type_filter,
    rule_type_ctrl,
    rule_type_reslib_custom_proto_dat,
    rule_type_reslib_custom_http_proto,
    rule_type_reslib_custom_proto_list,
    rule_type_reslib_pcap,
    rule_type_reslib_xdr_temp,
    rule_type_reslib_ipnet,
    rule_type_reslib_traffic,
    rule_type_xs_comm,
    rule_type_xs_tra,
    rule_type_xs_web,
    rule_type_xs_sess,
    rule_type_comm_increase,
    rule_type_comm_full,
    rule_type_tuple_increase,       ///< 五元组增量更新规则文件
    rule_type_tuple_full,           ///< 五元组全量更新规则文件
    rule_type_single_increase,      ///< 单匹配项增量更新规则文件
    rule_type_single_full,          ///< 单匹配项全量更新规则文件
    rule_type_flow_increase,        ///< flow.payload增量更新规则文件
    rule_type_flow_full,            ///< flow.payload全量更新规则文件
    rule_type_num,
}e_rule_type;

typedef struct {
    FILE           * fp;
    char             file_path[256];
    e_rule_type      type;
    u32              version;
    //u32              rule_idx;
}t_rule_file;

///< 主要是针对HTTP规则
typedef struct
{
    t_rule_file   *tuple_file_full;             ///< 五元组全量更新规则文件
    t_rule_file   *tuple_file_increase;         ///< 五元组增量更新规则文件
    t_rule_file   *singel_file_full;            ///< payload中，多匹配项在一个payload中,主要是针对HTTP规则
    t_rule_file   *singel_file_increase;        ///< payload中，多匹配项在一个payload中,主要是针对HTTP规则
    t_rule_file   *comm_file_full;              ///< 普通规则文件
    t_rule_file   *comm_file_increase;          ///< 普通规则文件
    t_rule_file   *flow_file_full;              ///< flow.payload规则文件
    t_rule_file   *flow_file_increase;          ///< flow.payload规则文件
}t_rule_file_ctx;

void plc_mkdirs(char *muldir);
int __rule_file_write(t_rule_file * file,void * data,int data_len);
bool __rule_file_append(t_rule_file * file,const char * f,...);
bool rule_file_append(t_rule_file * file,char * user,char * match,char * action, e_file_type file_type);

void rule_file_free(t_rule_file * f, e_file_type file_type);
void rule_file_complete(t_rule_file * f);

t_rule_file * rule_file_alloc(e_rule_type type, e_file_type file_type);

#endif
