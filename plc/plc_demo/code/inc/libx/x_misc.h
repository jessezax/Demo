#ifndef __X_MISC_H__
#define __X_MISC_H__

static inline bool pre_tail(u8 * pos,u8 * tail,u32 size)
{
    if (pos + size > tail) {
        return false;
    }
    return true;
}

int send_all_size(int fd,void * buf,int size);
int recv_all_size(int fd,void * buf,int size);

int m_poll(struct pollfd * pfd,int num,int timeout);
int m_accept(int listen_fd,struct sockaddr* sock_addr,socklen_t * len);
int m_connect(int sockfd,struct sockaddr* sock_addr,socklen_t addrlen);

int setblocking(int fd);

int setnonblocking(int fd);

int create_tcp_socket(void);

int create_tcp_socket_ipv6(void);

bool file_exist(const char * path);

off_t file_size(const char *path);

int file_mv(const char *src_path, const char *des_path);

int file_copy(const char *src_path, const char *des_path);

int file_put_contents(const char * file_path,off_t start,const char * buf,int length);

int file_get_contents(const char * file_path,off_t start,char * buf,int max_length);

int load_proc_info_by_pid(pid_t pid,const char * info,char * buf,int buf_len);

int load_exe_by_pid(pid_t pid,char * buf,int buf_len);

const char * load_self_exe();

u64 clock_monotonic_ns(void);

u64 clock_realtime_ns(void);

u32 clock_monotonic(void);

u32 clock_realtime(void);

u64 rand64(void);

static inline be128 hton128(u128 val)
{
    u8 * pos = (u8 *)&val;
    u8   idx;
    u8   tmp;
    for(idx = 0;idx < 8;idx++)
    {
        tmp = pos[idx];
        pos[idx] = pos[15-idx];
        pos[15-idx] = tmp;
    }
    return val;
}
#define ntoh128(val) hton128((val))
#define ntohll(x) (((uint64_t) ntohl(x)) << 32) + ntohl(x >> 32)
#define htonll(x) (((uint64_t) htonl(x)) << 32) + htonl(x >> 32)

static inline
u32 explode(char * from,u32 from_size,char separator,t_string * to,u32 to_cnt)
{
    char * tail;
    char * pos;
    char * tok;
    u32    cnt = 0;

    if (!from || !from_size) {
        return 0;
    }
    pos  = from;
    tail = from + from_size;

    tok = lnindex(pos, tail - pos, separator);
    while(tok) {
        if (cnt >= to_cnt) {
            return cnt;
        }
        set_string(&to[cnt], pos, tok - pos);
        cnt++;
        pos = tok + 1;
        tok = lnindex(pos, tail - pos, separator);
    }
    set_string(&to[cnt], pos, tok - pos);
    cnt++;
    return cnt;
}

static inline
u32 list_to_u32(char * from,u32 from_size,char separator,u32 * to,u32 to_cnt)
{
    char * tail;
    char * pos;
    char * tok;
    u32    cnt = 0;

    pos  = from;
    tail = from + from_size;

    tok = lnindex(pos, tail - pos, separator);
    while(tok) {
        if (cnt >= to_cnt) {
            return cnt;
        }
        if (atou32(pos, tok - pos, &to[cnt])) {
            cnt++;
        }
        pos = tok + 1;
        tok = lnindex(pos, tail - pos, separator);
    }
    if(atou32(pos, tail - pos, &to[cnt])) {
        cnt++;
    }
    return cnt;
}

///< len表示的是输出字符串的长度，包括:和结束符
static inline char *inet6_ntop(const void *addrptr, char *strptr, size_t len)
{
    const struct in6_addr * src = addrptr;
    if (len < sizeof("FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF")) {
        return NULL;
    }
    snprintf(strptr,len, "%04x:%04x:%04x:%04x:%04x:%04x:%04x:%04x",
                        ntohs(src->s6_addr16[0]), ntohs(src->s6_addr16[1]),
                        ntohs(src->s6_addr16[2]), ntohs(src->s6_addr16[3]),
                        ntohs(src->s6_addr16[4]), ntohs(src->s6_addr16[5]),
                        ntohs(src->s6_addr16[6]), ntohs(src->s6_addr16[7]));
    return strptr;
}


//根据IPv4 掩码获取ip地址的起始范围,参数ip_v4 是网络字节序
static inline
void ipnet_prefix_convert(u32 ip,int prefix,char *ip_start,char *ip_end,int size)
{
    u32 ipaddr = 0;
    u32 ip_s_h = 0,ip_e_h = 0,ip_s_n = 0,ip_e_n = 0;

    if(prefix == 0)
    {
        sprintf(ip_start,"0.0.0.0");
        sprintf(ip_end,"255.255.255.255");
        return ;
    }
    ipaddr = ntohl(ip);

    ip_s_h   = (ipaddr & (0xFFFFFFFF << (32 - prefix)));
    ip_e_h   = (ipaddr | (~(0xFFFFFFFF << (32 - prefix))));
    //ip_e_h   = (ip_s_h + ((rsn_u4)0xFFFFFFFF >> prefix));

    ip_s_n   = htonl(ip_s_h);
    ip_e_n   = htonl(ip_e_h);

    inet_ntop(AF_INET,&((ip_s_n)),ip_start,size);
    inet_ntop(AF_INET,&((ip_e_n)),ip_end,size);
}

//根据IPv6 掩码获取ip地址的起始范围,参数ip_v6 是网络字节序
static inline
void ip6net_prefix_convert(u128 ip6,int prefix,char *ip_start, char *ip_end,int size)
{
    int i,l_len;
    u8  *ip_v6 = (u8 *)&ip6;
    char ip_v6_s[16] = {0},ip_v6_e[16] = {0};

    l_len = prefix / 8;
    //inet_pton(AF_INET6,ipv6_str,&ip_v6);

    for(i=0; i<l_len;i++) {
        ip_v6_s[i] = ip_v6[i];
        ip_v6_e[i] = ip_v6[i];
    }

    if (prefix % 8) {
        ip_v6_s[i] = ip_v6[i] & 0xFF << (8 - prefix % 8);
        ip_v6_e[i] = ip_v6[i] | ~(0xFF << (8 - prefix % 8));
        i++;
    }

    for(; i< 16;i++) {
        ip_v6_s[i] = 0x00;
        ip_v6_e[i] = 0xFF;
    }

    inet6_ntop(ip_v6_s,ip_start, size);
    inet6_ntop(ip_v6_e,ip_end, size);
}

#define ARRAY_SIZE(array) (sizeof(array)/sizeof(array[0]))

typedef struct {
    char from;
    u8   to_len;
    char to[6];
}t_escape;

static inline
t_escape * escape_find(char c,t_escape * tab,int tab_size)
{
    int idx;
    for (idx = 0;idx < tab_size;idx++) {
        if (c == tab[idx].from) {
            return &tab[idx];
        }
    }
    return NULL;
}

static inline int escape(char *in,int in_size, char * out, int out_size,t_escape * tab,int tab_size)
{
    int in_idx;
    int out_idx = 0;
    t_escape * esc;
    for (in_idx = 0;in_idx < in_size && out_idx < out_size;in_idx++) {
        esc = escape_find(in[in_idx],tab,tab_size);
        if (esc) {
            if (out_idx + esc->to_len >= out_size) {
                break;
            }
            memcpy(out + out_idx,esc->to,esc->to_len);
            out_idx += esc->to_len;
        } else {
            out[out_idx] = in[in_idx];
            out_idx++;
        }
    }
    if (out_idx < out_size) {
        out[out_idx] = '\0';
    } else if (out_size > 0) {
        out[out_size - 1] = '\0';
    }
    return out_idx;
}

static inline int unregex_escape(char * in,int in_size, char * out, int out_size)
{
    static t_escape tab[] = {
        {
            '\\',const_strlen("\\\\"),"\\\\"
        },{
            '|',const_strlen("\\|"),"\\|"
        },{
            '.',const_strlen("\\."),"\\."
        },{
            '?',const_strlen("\\?"),"\\?"
        },{
            '+',const_strlen("\\+"),"\\+"
        },{
            '*',const_strlen("\\*"),"\\*"
        },{
            '$',const_strlen("\\$"),"\\$"
        },{
            '^',const_strlen("\\^"),"\\^"
        },{
            '(',const_strlen("\\("),"\\("
        },{
            ')',const_strlen("\\)"),"\\)"
        },{
            '[',const_strlen("\\["),"\\["
        },{
            ']',const_strlen("\\]"),"\\]"
        },{
            '{',const_strlen("\\{"),"\\{"
        },{
            '}',const_strlen("\\}"),"\\}"
        }
    };

    return escape(in, in_size, out, out_size,tab, ARRAY_SIZE(tab));
}

///< 将time_t类型的时间转化为字符串
static inline void
time2str(time_t time, char *time_str, int str_len)
{
    struct tm  st_tm;
    localtime_r(&time,&st_tm);
    strftime(time_str,str_len,"%Y-%m-%d %H:%M:%S",&st_tm);
}

static inline
int comm_poll(struct pollfd *pfd, int num, int timeout)
{
    int ret;
loop:
    ret = poll(pfd, num, timeout);
    if(ret < 0){
        if(errno == EINTR){
            goto loop;
        }
        printf("poll err: [%s]", strerror(errno));
        return -1;
    }
    return ret;
}

#endif
