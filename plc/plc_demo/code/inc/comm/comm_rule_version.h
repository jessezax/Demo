#ifndef __comm_RULE_VERSION_H__
#define __comm_RULE_VERSION_H__

typedef struct {
    char             rule_name[64];
    char             rule_filename[64];
    char             rule_path[256];
    char             version_path[256];
    e_rule_type      type;
    e_comm_plc_type  plc_type;
    u32              version;
    u32              tmp_version;
}t_rule_version;

u32 rule_version(e_rule_type type);
u32 rule_version_new(e_rule_type type);
bool rule_version_update(t_rule_file * file, e_file_type file_type);
u32 get_file_size(const char* file);

#endif
