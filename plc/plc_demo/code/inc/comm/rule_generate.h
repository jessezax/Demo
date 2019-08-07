#ifndef __RULE_GENERATE_H__
#define __RULE_GENERATE_H__

extern u32 success;

///< 消息类型代码
typedef enum {
    cmb_comm_type       = 0x00, ///< pkt.payload~"A" && pkt.payload~"B"
    cmb_cmb_type        = 0x01, ///< pkt.payload~"‘A’ & 'B'"
    cmb_flow_type       = 0x02, ///< flow.payload~"A" && flow.payload~"B"
}e_cmb_type;

typedef struct
{
    u8                  user_id;                ///< 登陆用户ID信息
    u8                  port_group;             ///< 输出端口组号
    u8                  act;                    ///< 动作，0-丢弃，1-转发
    u8                  save;                   ///< 重启保存，0-不保存，1-保存
    u8                  hst;                    ///< 中标统计，0-不统计，1-统计
    u8                  single;                 ///< 单包匹配标记位，1单包，0跨包
    u8                  isp;                    ///< 运营商
    u8                  rule_type;              ///< 规则类型
    u8                  pro_id;                 ///< 协议ID，只有五元组的action部分此字段有效，其他默认为0
    u64                 app_id;                 ///< 应用ID
    u32                 rule_id;                ///< 规则ID
    u8                  oft;                    ///< 偏移类型，仅仅对特征码有效，1表示IP，2表示TCP，3表示UDP，0表示无
    u8                  tuple_oft;              ///< 标记五元组的类型，和oft一致
    char                pro[16];                ///< 协议字段, 仅仅对应用和协议有效
    char                start_time[32];         ///< 时间戳
}t_action_fields;

typedef struct
{
    u8                  multi;                  ///< 标记payload多个匹配项，是否合成一个匹配项
    e_cmb_type          cmb_flag;               ///< 标记当前是从采用哪种方式填充match的, 0表示默认，1表示cmb，2表示flow
    t_action_fields     action;
    char                match[0];
}t_rule_generate_one;

typedef struct
{
    u8  comm_flag;          ///< pkt.payload~"A" && pkt.payload~"B"
    u8  flow_flag;          ///< flow.payload~"A" && flow.payload~"B"
}t_cmb_flag;

extern  u64         g_rule_start_idx;           ///< 全量更新规则ID起始值
extern  u64         g_rule_idx;                 ///< 全量更新规则ID
extern  u64         g_rule_increase_idx;        ///< 增量更新规则规则ID

extern  u64         curr_increase_rule_num;     ///< 记录更新当前收到的规则数
extern  u64         curr_full_rule_num;         ///< 当前需要全量更新的规则总数
extern  t_cmb_flag  g_cmb_flag;                 ///< 多匹配项组合规则是否单独生成文件

bool need_escape(char ch);

/*
 * ###############################################################
 * 将规则插入到规则文件中
 * ###############################################################
 */
bool comm_rule_generate(t_rule_generate_one *rule, t_rule_file *rule_file, e_action_type op, e_file_type file_type);
/*
 * ###############################################################
 * 根据掩码,生成对应的正则表达式
 * ###############################################################
 */
bool generate_comm_match_from_mask(char *src, u8 *mask, u32 len, char **pos, char **tail, u8 encode_method, int offset);

/*
 * ###################################################################
 * MATCH部分生成, 用于组合和非组合规则,以及普通规则和flow.payload规则
 * ###################################################################
 */
bool condi_code_match_generate( char *match_pos,
                                char *match_tail,
                                t_cond_code_info *info,
                                bool start_flag,
                                bool not_flag,
                                e_rule_file_type type);

bool ip_port_pro_match_generate(    u8 ip_flag,
                                    char *match_pos,
                                    char *match_tail,
                                    t_ip_port_pro_info *info,
                                    bool start_flag,
                                    bool not_flag,
                                    e_rule_file_type type);

bool account_match_generate(    char * match_pos,
                                char * match_tail,
                                t_account_info * info,
                                bool start_flag,
                                bool not_flag,
                                e_rule_file_type type);

bool audio_video_match_up_generate( char *match_pos,
                                    char *match_tail,
                                    t_audio_video_info *info,
                                    bool start_flag,
                                    bool not_flag,
                                    e_rule_file_type type);

bool audio_video_match_down_generate(   char *match_pos,
                                        char *match_tail,
                                        t_audio_video_info *info,
                                        bool start_flag,
                                        bool not_flag,
                                        e_rule_file_type type);

bool pro_app_match_generate(    char *match_pos,
                                char *match_tail,
                                t_pro_app_type_info *info,
                                bool start_flag,
                                bool not_flag,
                                e_rule_file_type type);

bool http_match_up_generate(    char *match_pos,
                                char *match_tail,
                                t_http_info *info,
                                bool start_flag,
                                bool not_flag,
                                e_rule_file_type type);

bool http_match_down_generate(  char *match_pos,
                                char *match_tail,
                                t_http_info *info,
                                bool start_flag,
                                bool not_flag,
                                e_rule_file_type type);

bool dns_match_generate(    char *match_pos,
                            char *match_tail,
                            t_dns_info *info,
                            bool start_flag,
                            bool not_flag);

bool model_match_generate(  char *match_pos,
                            char *match_tail,
                            t_model_info *info,
                            bool start_flag,
                            bool not_flag);


/*
 * ###############################################################
 * 规则生成, 用于增量和全量更新
 * ###############################################################
 */
bool __account_rule_generate(       t_rule_file_ctx *ctx,
                                    t_handle_msg_info *msg,
                                    e_action_type op,
                                    e_file_type file_type,
                                    e_rule_file_type type);

bool __audio_video_rule_generate(   t_rule_file_ctx *ctx,
                                    t_handle_msg_info *msg,
                                    e_action_type op,
                                    e_file_type file_type,
                                    e_rule_file_type type);

bool __condi_code_rule_generate(    t_rule_file_ctx *ctx,
                                    t_handle_msg_info *msg,
                                    e_action_type op,
                                    e_file_type file_type,
                                    e_rule_file_type type);

bool __dns_rule_generate(           t_rule_file_ctx *ctx,
                                    t_handle_msg_info *msg,
                                    e_action_type op,
                                    e_file_type file_type,
                                    e_rule_file_type type);

bool __http_rule_generate(          t_rule_file_ctx *ctx,
                                    t_handle_msg_info *msg,
                                    e_action_type op,
                                    e_file_type file_type,
                                    e_rule_file_type type);

bool __model_rule_generate(         t_rule_file_ctx *ctx,
                                    t_handle_msg_info *msg,
                                    e_action_type op,
                                    e_file_type file_type,
                                    e_rule_file_type type);

bool __ip_port_pro_rule_generate(   t_rule_file_ctx *ctx,
                                    t_handle_msg_info *msg,
                                    e_action_type op,
                                    e_file_type file_type,
                                    e_rule_file_type type);

bool __pro_app_rule_generate(       t_rule_file_ctx *ctx,
                                    t_handle_msg_info *msg,
                                    e_action_type op,
                                    e_file_type file_type,
                                    e_rule_file_type type);

bool __cmb_rule_generate(       t_rule_file_ctx         *ctx,
                                t_handle_msg_info       *msg,
                                e_action_type           op,
                                e_file_type             file_type,
                                e_rule_file_type        type);

/*
 * ###############################################################
 * 全量更新各类规则文件
 * ###############################################################
 */


/*
 * ###############################################################
 * 增量更新规则文件
 * ###############################################################
 */
void rule_generate_for_inc( t_connection * conn,
                            e_comm_msg_type msg_type,
                            e_action_type op,
                            t_handle_msg_info *handle_msg,
                            e_rule_file_type type,
                            void *arg);

void rule_file_ctx_destroy(t_rule_file_ctx *ctx, e_file_type file_type);

/*
 * ###############################################################
 * 文件相关操作
 * ###############################################################
 */
bool rule_file_ctx_init(t_rule_file_ctx *ctx, e_file_type file_type);
bool file_ctx_complete(t_rule_file_ctx *ctx, e_file_type file_type);
bool file_ctx_update(t_rule_file_ctx *ctx, e_file_type file_type);
void rule_generate();



#endif
