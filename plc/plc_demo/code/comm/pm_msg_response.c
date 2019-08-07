#include "libx.h"
#include "comm.h"

u64 inq_notify(u8 user_id, u32 rule_id)
{
    socklen_t addrlen = 0;
    int sockfd = -1;
    int ret;
    char buf[1024]  = {0};
    struct sockaddr_in dpi_addr;
    u64     hit_counts;

    if(0 == inet_pton(AF_INET, dpi_inq_addr, &dpi_addr.sin_addr))
    {
        return false;
    }
    dpi_addr.sin_port = htons(dpi_inq_port);
    dpi_addr.sin_family = AF_INET;

    //snprintf(buf, 1024, "mirr rule_hitstat %u", rule_id);
    snprintf(buf, 1024, "%u%u", user_id, rule_id);
    addrlen= sizeof(struct sockaddr_in);
    sockfd = create_tcp_socket();

    if (sockfd < 0) {
        return false;
    }
    if (m_connect(sockfd, (struct sockaddr *)&dpi_addr, addrlen) < 0) {
        goto connect_error;
    }

    ret = send_all_size(sockfd, buf, strlen(buf));
    if(ret != strlen(buf))
    {
        perror("send to dpi error:");
        goto send_error;
    }

    bzero(buf, 1024);
    ret = recv_all_size(sockfd, buf, 64);
    if(ret < 0)
    {
        perror("recv from dpi error:");
        goto recv_error;
    }
    printf("recv from dpi info is: %s\n", buf);

    hit_counts = atoll(buf + 3);
    return hit_counts;
recv_error:
send_error:
connect_error:
    close(sockfd);
    return 0;
}

