#ifndef __comm_RULE_RESPONSE_H__
#define __comm_RULE_RESPONSE_H__

extern char dpi_inq_addr[128];
extern u16 dpi_inq_port;

u64 inq_notify(u8 user_id, u32 rule_id);

#endif
