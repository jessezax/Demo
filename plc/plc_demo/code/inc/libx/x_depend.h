#ifndef __X_DEPAND_H__
#define __X_DEPAND_H__

#ifdef X_DEPAND

typedef uint32_t in_addr_t;
struct in_addr
{
    in_addr_t s_addr;
};

typedef uint16_t in_port_t;

/* POSIX.1g specifies this type name for the `sa_family' member.  */
typedef unsigned short int sa_family_t;

/* This macro is used to declare the initial common members
   of the data types used for socket addresses, `struct sockaddr',
   `struct sockaddr_in', `struct sockaddr_un', etc.  */

#define __SOCKADDR_COMMON(sa_prefix) sa_family_t sa_prefix##family

#define __SOCKADDR_COMMON_SIZE  (sizeof (unsigned short int))

struct sockaddr_in
{
    __SOCKADDR_COMMON (sin_);
    in_port_t sin_port;                 /* Port number.  */
    struct in_addr sin_addr;            /* Internet address.  */

    /* Pad to size of `struct sockaddr'.  */
    unsigned char sin_zero[sizeof(struct sockaddr) -__SOCKADDR_COMMON_SIZE - sizeof(in_port_t) - sizeof(struct in_addr)];
};

/* IPv6 address */
struct in6_addr
{
    union
    {
        uint8_t __u6_addr8[16];
        uint16_t __u6_addr16[8];
        uint32_t __u6_addr32[4];
    } __in6_u;
#define s6_addr                __in6_u.__u6_addr8
#define s6_addr16              __in6_u.__u6_addr16
#define s6_addr32              __in6_u.__u6_addr32
};

/* Ditto, for IPv6.  */
struct sockaddr_in6
{
    __SOCKADDR_COMMON (sin6_);
    in_port_t sin6_port;        /* Transport layer port # */
    uint32_t sin6_flowinfo;     /* IPv6 flow information */
    struct in6_addr sin6_addr;  /* IPv6 address */
    uint32_t sin6_scope_id;     /* IPv6 scope-id */
};

struct ip_mreq
{
    /* IP multicast address of group.  */
    struct in_addr imr_multiaddr;

    /* Local IP address of interface.  */
    struct in_addr imr_interface;
};

struct ipv6_mreq
{
    /* IPv6 multicast address of group */
    struct in6_addr ipv6mr_multiaddr;

    /* local interface */
    unsigned int ipv6mr_interface;
};


/* Standard well-defined IP protocols.  */
enum
{
    IPPROTO_IP = 0,        /* Dummy protocol for TCP.  */
#define IPPROTO_IP              IPPROTO_IP
    IPPROTO_ICMP = 1,      /* Internet Control Message Protocol.  */
#define IPPROTO_ICMP            IPPROTO_ICMP
    IPPROTO_IGMP = 2,      /* Internet Group Management Protocol. */
#define IPPROTO_IGMP            IPPROTO_IGMP
    IPPROTO_IPIP = 4,      /* IPIP tunnels (older KA9Q tunnels use 94).  */
#define IPPROTO_IPIP            IPPROTO_IPIP
    IPPROTO_TCP = 6,       /* Transmission Control Protocol.  */
#define IPPROTO_TCP             IPPROTO_TCP
    IPPROTO_EGP = 8,       /* Exterior Gateway Protocol.  */
#define IPPROTO_EGP             IPPROTO_EGP
    IPPROTO_PUP = 12,      /* PUP protocol.  */
#define IPPROTO_PUP             IPPROTO_PUP
    IPPROTO_UDP = 17,      /* User Datagram Protocol.  */
#define IPPROTO_UDP             IPPROTO_UDP
    IPPROTO_IDP = 22,      /* XNS IDP protocol.  */
#define IPPROTO_IDP             IPPROTO_IDP
    IPPROTO_TP = 29,       /* SO Transport Protocol Class 4.  */
#define IPPROTO_TP              IPPROTO_TP
    IPPROTO_DCCP = 33,     /* Datagram Congestion Control Protocol.  */
#define IPPROTO_DCCP            IPPROTO_DCCP
    IPPROTO_IPV6 = 41,     /* IPv6 header.  */
#define IPPROTO_IPV6            IPPROTO_IPV6
    IPPROTO_RSVP = 46,     /* Reservation Protocol.  */
#define IPPROTO_RSVP            IPPROTO_RSVP
    IPPROTO_GRE = 47,      /* General Routing Encapsulation.  */
#define IPPROTO_GRE             IPPROTO_GRE
    IPPROTO_ESP = 50,      /* encapsulating security payload.  */
#define IPPROTO_ESP             IPPROTO_ESP
    IPPROTO_AH = 51,       /* authentication header.  */
#define IPPROTO_AH              IPPROTO_AH
    IPPROTO_MTP = 92,      /* Multicast Transport Protocol.  */
#define IPPROTO_MTP             IPPROTO_MTP
    IPPROTO_BEETPH = 94,   /* IP option pseudo header for BEET.  */
#define IPPROTO_BEETPH          IPPROTO_BEETPH
    IPPROTO_ENCAP = 98,    /* Encapsulation Header.  */
#define IPPROTO_ENCAP           IPPROTO_ENCAP
    IPPROTO_PIM = 103,     /* Protocol Independent Multicast.  */
#define IPPROTO_PIM             IPPROTO_PIM
    IPPROTO_COMP = 108,    /* Compression Header Protocol.  */
#define IPPROTO_COMP            IPPROTO_COMP
    IPPROTO_SCTP = 132,    /* Stream Control Transmission Protocol.  */
#define IPPROTO_SCTP            IPPROTO_SCTP
    IPPROTO_UDPLITE = 136, /* UDP-Lite protocol.  */
#define IPPROTO_UDPLITE         IPPROTO_UDPLITE
    IPPROTO_MPLS = 137,    /* MPLS in IP.  */
#define IPPROTO_MPLS            IPPROTO_MPLS
    IPPROTO_RAW = 255,     /* Raw IP packets.  */
#define IPPROTO_RAW             IPPROTO_RAW
    IPPROTO_MAX
};


char * strtok (char *str, const char * delimiters);
char *strtok_r(char *str, const char *delim, char **saveptr);
void * memmove( void * destination, const void * source, size_t num );


int   pthread_spin_destroy(pthread_spinlock_t *);
int   pthread_spin_init(pthread_spinlock_t *, int);
int   pthread_spin_lock(pthread_spinlock_t *);
int   pthread_spin_trylock(pthread_spinlock_t *);
int   pthread_spin_unlock(pthread_spinlock_t *);


int   pthread_create(pthread_t *restrict, const pthread_attr_t *restrict,void *(*)(void *), void *restrict);
int   pthread_detach(pthread_t);
int   pthread_equal(pthread_t, pthread_t);
void  pthread_exit(void *);
int   pthread_join(pthread_t, void **);

int   pthread_mutex_destroy(pthread_mutex_t *);
int   pthread_mutex_getprioceiling(const pthread_mutex_t *restrict,
          int *restrict);
int   pthread_mutex_init(pthread_mutex_t *restrict,
          const pthread_mutexattr_t *restrict);
int   pthread_mutex_lock(pthread_mutex_t *);
int   pthread_mutex_setprioceiling(pthread_mutex_t *restrict, int,
          int *restrict);
int   pthread_mutex_timedlock(pthread_mutex_t *,
          const struct timespec *);
int   pthread_mutex_trylock(pthread_mutex_t *);
int   pthread_mutex_unlock(pthread_mutex_t *);
int   pthread_mutexattr_destroy(pthread_mutexattr_t *);
int   pthread_mutexattr_getprioceiling(
          const pthread_mutexattr_t *restrict, int *restrict);
int   pthread_mutexattr_getprotocol(const pthread_mutexattr_t *restrict,
          int *restrict);
int   pthread_mutexattr_getpshared(const pthread_mutexattr_t *restrict,
          int *restrict);
int   pthread_mutexattr_gettype(const pthread_mutexattr_t *restrict,
          int *restrict);
int   pthread_mutexattr_init(pthread_mutexattr_t *);
int   pthread_mutexattr_setprioceiling(pthread_mutexattr_t *, int);
int   pthread_mutexattr_setprotocol(pthread_mutexattr_t *, int);
int   pthread_mutexattr_setpshared(pthread_mutexattr_t *, int);
int   pthread_mutexattr_settype(pthread_mutexattr_t *, int);

struct timespec{
    time_t tv_sec;  /*seconds 秒*/
    long   tv_nsec; /*nanoseconds 纳秒*/
};

#define CLOCK_REALTIME
        //System-wide clock that measures real (i.e., wall-clock) time.
        //Setting this clock requires appropriate privileges.  This
        //clock is affected by discontinuous jumps in the system time
        //(e.g., if the system administrator manually changes the
        //clock), and by the incremental adjustments performed by
        //adjtime(3) and NTP.

#define CLOCK_REALTIME_COARSE
        //(since Linux 2.6.32; Linux-specific)
        //A faster but less precise version of CLOCK_REALTIME.  Use when
        //you need very fast, but not fine-grained timestamps.  Requires
        //per-architecture support, and probably also architecture sup‐
        //port for this flag in the vdso(7).

#define CLOCK_MONOTONIC
        //Clock that cannot be set and represents monotonic time since
        //some unspecified starting point.  This clock is not affected
        //by discontinuous jumps in the system time (e.g., if the system
        //administrator manually changes the clock), but is affected by
        //the incremental adjustments performed by adjtime(3) and NTP.

#define CLOCK_MONOTONIC_COARSE
        //(since Linux 2.6.32; Linux-specific)
        //A faster but less precise version of CLOCK_MONOTONIC.  Use
        //when you need very fast, but not fine-grained timestamps.
        //Requires per-architecture support, and probably also architec‐
        //ture support for this flag in the vdso(7).

#define CLOCK_MONOTONIC_RAW
        //(since Linux 2.6.28; Linux-specific)
        //Similar to CLOCK_MONOTONIC, but provides access to a raw hard‐
        //ware-based time that is not subject to NTP adjustments or the
        //incremental adjustments performed by adjtime(3).

#define CLOCK_BOOTTIME
        //(since Linux 2.6.39; Linux-specific)
        //Identical to CLOCK_MONOTONIC, except it also includes any time
        //that the system is suspended.  This allows applications to get
        //a suspend-aware monotonic clock without having to deal with
        //the complications of CLOCK_REALTIME, which may have disconti‐
        //nuities if the time is changed using settimeofday(2) or similar.

#define CLOCK_PROCESS_CPUTIME_ID
        //(since Linux 2.6.12)
        //Per-process CPU-time clock (measures CPU time consumed by all
        //threads in the process).

#define CLOCK_THREAD_CPUTIME_ID
        //(since Linux 2.6.12)
        //Thread-specific CPU-time clock.

int clock_getres(clockid_t clk_id, struct timespec *res);

int clock_gettime(clockid_t clk_id, struct timespec *tp);

int clock_settime(clockid_t clk_id, const struct timespec *tp);


#define AF_INET
#define AF_INET6
#include <arpe/inet.h>
//返回值：若成功则为1，若输入不是有效的表达式则为0，若出错则为-1
//将点分十进制的ip地址转化为用于网络传输的数值格式
int inet_pton(int family, const char *strptr, void *addrptr);

//将数值格式转化为点分十进制的ip地址格式
//返回值：若成功则为指向结构的指针，若出错则为NULL
const char * inet_ntop(int family, const void *addrptr, char *strptr, size_t len);

int rename(const char *oldname, const char *newname);
int remove(const char * pathname);

#endif

#endif

