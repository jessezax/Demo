#include "libx.h"
#include <netinet/tcp.h>
#include <curl/curl.h>


int recv_all_size(int fd,void * buf,int size)
{
    int recv_len;
    int remain;
    char * pos;
    pos = buf;
    remain = size;

    while(remain) {
        recv_len = recv(fd,pos,remain,MSG_WAITALL);
        if (recv_len < 0) {
            if (errno == EINTR || errno == EAGAIN) {
                continue;
            }
            return -1;
        } else if (recv_len == 0) {
            return size - remain;
        }
        pos += recv_len;
        remain -= recv_len;
    }
    return size;
}

int send_all_size(int fd,void * buf,int size)
{
    int send_len;
    int remain;
    char * pos;
    pos = buf;
    remain = size;

    do
    {
        send_len = send(fd,pos,remain,MSG_NOSIGNAL|MSG_WAITALL);
        if (send_len < 0)
        {
            if (errno == EINTR || errno == EAGAIN)
            {
                continue;
            }
            perror("socket error: ");
            return -1;
        }
        else if (send_len == 0)
        {
            return size - remain;
        }
        pos += send_len;
        remain -= send_len;
    }while (remain);
    return size;
}

int m_poll(struct pollfd * pfd,int num,int timeout)
{
    int ret;
loop:
    ret = poll(pfd,num,timeout);
    if (ret < 0)
    {
        if (errno == EINTR)
        {
            goto loop;
        }
        return -1;
    }
    return ret;
}

int m_accept(int listen_fd,struct sockaddr* sock_addr,socklen_t * len)
{
    int ret;
loop:
    ret = accept(listen_fd,sock_addr,len);
    if (ret < 0)
    {
        if (errno == EINTR)
        {
            goto loop;
        }
        return -1;
    }
    return ret;
}

int m_connect(int sockfd,struct sockaddr* sock_addr,socklen_t addrlen)
{
    int ret;
loop:
    ret = connect(sockfd,sock_addr,addrlen);
    if (ret < 0)
    {
        if (errno == EINTR)
        {
            goto loop;
        }
        return -1;
    }
    return ret;
}

int setblocking(int fd)
{
    int opts;
    opts=fcntl(fd,F_GETFL);
    if(opts<0)
    {
        perror("fcntl(sock,GETFL)");
        return -1;
    }
    opts = opts&(~O_NONBLOCK);
    if(fcntl(fd,F_SETFL,opts)<0)
    {
        perror("fcntl(sock,SETFL,opts)");
        return -2;
    }
    return 0;
}

int setnonblocking(int fd)
{
    int opts;
    opts=fcntl(fd,F_GETFL);
    if(opts<0)
    {
        perror("fcntl(sock,GETFL)");
        return -1;
    }
    opts = opts|O_NONBLOCK;
    if(fcntl(fd,F_SETFL,opts)<0)
    {
        perror("fcntl(sock,SETFL,opts)");
        return -2;
    }
    return 0;
}

int create_tcp_socket_ipv6(void)
{
    int sockfd;

    sockfd = socket(AF_INET6,SOCK_STREAM,0);
    if (sockfd < 0)
    {
        //printf("create socket err : %s",strerror(errno));
        return -1;;
    }
    {
        int keepAlive = 1;
        int keepIdle = 15;
        int keepInterval = 5;
        int keepCount = 9;
        if (setsockopt(sockfd, SOL_SOCKET, SO_KEEPALIVE, (void*)&keepAlive, sizeof(keepAlive)) < 0)
        {
            //printf("asyn_connect setsockopt SO_KEEPALIVE : %s",strerror(errno));
        }
        if (setsockopt(sockfd, SOL_TCP, TCP_KEEPIDLE, (void *)&keepIdle, sizeof(keepIdle)) < 0)
        {
            //printf("asyn_connect setsockopt TCP_KEEPIDLE : %s",strerror(errno));
        }
        if (setsockopt(sockfd, SOL_TCP, TCP_KEEPINTVL, (void *)&keepInterval, sizeof(keepInterval)) < 0)
        {
            //printf("asyn_connect setsockopt TCP_KEEPINTVL : %s",strerror(errno));
        }
        if (setsockopt(sockfd, SOL_TCP, TCP_KEEPCNT, (void *)&keepCount, sizeof(keepCount)) < 0)
        {
            //printf("asyn_connect setsockopt TCP_KEEPCNT : %s",strerror(errno));
        }
    }
    return sockfd;
}

int create_tcp_socket(void)
{
    int sockfd;

    sockfd = socket(AF_INET,SOCK_STREAM,0);
    if (sockfd < 0)
    {
        //printf("create socket err : %s",strerror(errno));
        return -1;;
    }
    {
        int keepAlive = 1;
        int keepIdle = 15;
        int keepInterval = 5;
        int keepCount = 9;
        if (setsockopt(sockfd, SOL_SOCKET, SO_KEEPALIVE, (void*)&keepAlive, sizeof(keepAlive)) < 0)
        {
            //printf("asyn_connect setsockopt SO_KEEPALIVE : %s",strerror(errno));
        }
        if (setsockopt(sockfd, SOL_TCP, TCP_KEEPIDLE, (void *)&keepIdle, sizeof(keepIdle)) < 0)
        {
            //printf("asyn_connect setsockopt TCP_KEEPIDLE : %s",strerror(errno));
        }
        if (setsockopt(sockfd, SOL_TCP, TCP_KEEPINTVL, (void *)&keepInterval, sizeof(keepInterval)) < 0)
        {
            //printf("asyn_connect setsockopt TCP_KEEPINTVL : %s",strerror(errno));
        }
        if (setsockopt(sockfd, SOL_TCP, TCP_KEEPCNT, (void *)&keepCount, sizeof(keepCount)) < 0)
        {
            //printf("asyn_connect setsockopt TCP_KEEPCNT : %s",strerror(errno));
        }
    }
    return sockfd;
}

bool file_exist(const char * path)
{
    FILE * fp;
    fp = fopen(path,"r");
    if (!fp)
    {
        return false;
    }
    fclose(fp);
    return true;
}

off_t file_size(const char *path)
{
    struct stat buf;
    if (stat(path, &buf) < 0)
    {
        return -1;
    }
    return buf.st_size;
}

/*
 * 文件移动
 * @param       src_path            源文件
 * @param       des_path            目的文件
 * @return      0:成功，-1:源文件打开失败，-2:目的文件打开失败，-3:文件发送拷贝失败
 */
int file_mv(const char *src_path, const char *des_path)
{
    int fd_src, fd_des;
    ssize_t send_file;
    struct stat stat_buf;

    fd_src = open(src_path, O_RDONLY);
    if(fd_src < 0){
        goto src_error;
    }

    fstat(fd_src, &stat_buf);

    fd_des = open(des_path, O_WRONLY | O_CREAT | O_TRUNC, 0666);
    if(fd_des < 0){
        goto des_error;
    }

    send_file = sendfile(fd_des, fd_src, NULL, stat_buf.st_size);
    if(send_file != stat_buf.st_size){
        goto send_file_error;
    }
    close(fd_des);
    remove(src_path);
    return 0;

src_error:
    printf("open src_file error %s\n",strerror(errno));
    return -1;
des_error:
    printf("open des_file error %s\n",strerror(errno));
    close(fd_src);
    return -2;
send_file_error:
    printf("send file error %s\n",strerror(errno));
    close(fd_src);
    close(fd_des);
    return -3;
}

/*
 * 文件拷贝
 * @param       src_path            源文件
 * @param       des_path            目的文件
 * @return      0:成功，-1:源文件打开失败，-2:目的文件打开失败，-3:文件发送拷贝失败
 */
int file_copy(const char *src_path, const char *des_path)
{
    int fd_src, fd_des;
    ssize_t send_file;
    struct stat stat_buf;

    fd_src = open(src_path, O_RDONLY);
    if(fd_src < 0){
        goto src_error;
    }

    fstat(fd_src, &stat_buf);

    fd_des = open(des_path, O_WRONLY | O_CREAT | O_TRUNC, 0666);
    if(fd_des < 0){
        goto des_error;
    }

    send_file = sendfile(fd_des, fd_src, NULL, stat_buf.st_size);
    if(send_file != stat_buf.st_size){
        goto send_file_error;
    }
    close(fd_des);
    return 0;

src_error:
    printf("open src_file error %s\n",strerror(errno));
    return -1;
des_error:
    printf("open des_file error %s\n",strerror(errno));
    close(fd_src);
    return -2;
send_file_error:
    printf("send file error %s\n",strerror(errno));
    close(fd_src);
    close(fd_des);
    return -3;
}


int file_put_contents(const char * file_path,off_t start,const char * buf,int length)
{
    FILE * fp;
    const char * pos;
    int total_len;
    int remain_len;
    int read_len;

    fp = fopen(file_path,"w+");
    if (!fp)
    {
        return -1;
    }

    pos = buf;
    read_len = 0;
    total_len = 0;
    remain_len = length;

    fseek(fp,start,SEEK_SET);
    while (remain_len > 0)
    {
        read_len = fwrite(pos,1,remain_len,fp);
        total_len += read_len;
        pos += read_len;

        if (read_len < remain_len)
        {
            if (ferror(fp))
            {
                total_len = -1;
                break;
            }
        }
        remain_len -= read_len;
    }

    fclose(fp);
    return total_len;
}

int file_get_contents(const char * file_path,off_t start,char * buf,int max_length)
{
    FILE * fp;
    char * pos;
    int total_len;
    int remain_len;
    int read_len;
    fp = fopen(file_path,"r");
    if (!fp)
    {
        return -1;
    }
    pos = buf;
    read_len = 0;
    total_len = 0;
    remain_len = max_length;

    fseek(fp,start,SEEK_SET);
    while (remain_len > 0)
    {
        read_len = fread(pos,1,remain_len,fp);
        total_len += read_len;
        pos += read_len;

        if (read_len < remain_len)
        {
            if (feof(fp))
            {
                break;
            }
            if (ferror(fp))
            {
                total_len = -1;
                break;
            }
        }
        remain_len -= read_len;
    }

    fclose(fp);

    return total_len;
}

int load_proc_info_by_pid(pid_t pid,const char * info,char * buf,int buf_len)
{
    char proc_file[128];
    int len;
    sprintf(proc_file,"/proc/%d/%s",pid,info);

    len = file_get_contents(proc_file,0,buf,buf_len);
    if (len >= 0) {
        buf[len] = 0;
    }
    return len;
}

int load_exe_by_pid(pid_t pid,char * buf,int buf_len)
{
    int len;
    char proc_file[128];

    sprintf(proc_file,"/proc/%d/exe",pid);

    if (!file_exist(proc_file))
    {
        return -1;
    }

    len = readlink(proc_file, buf, buf_len);
    if (len < 0)
    {
        return -2;
    }
    if (len >= buf_len)
    {
        return -2;
    }
    buf[len] = 0;
    return len;
}

const char * load_self_exe()
{
    int len;
    static char exe_path[1024];

    if (!file_exist("/proc/self/exe"))
    {
        return NULL;
    }

    len = readlink("/proc/self/exe", exe_path, 1024);
    if (len < 0)
    {
        return NULL;
    }
    if (len >= 1024)
    {
        return NULL;
    }
    exe_path[len] = 0;
    return exe_path;
}


u64 clock_monotonic_ns(void)
{
    struct timespec    now_tsp;
    u64 now_ns;
    clock_gettime(CLOCK_MONOTONIC, &now_tsp);
    now_ns = (u64)now_tsp.tv_nsec + (u64)(now_tsp.tv_sec *1000000000);
    return now_ns;
}

u64 clock_realtime_ns(void)
{
    struct timespec    now_tsp;
    u64 now_ns;
    clock_gettime(CLOCK_REALTIME, &now_tsp);
    now_ns = (u64)now_tsp.tv_nsec + (u64)(now_tsp.tv_sec *1000000000);
    return now_ns;
}

u32 clock_monotonic(void)
{
    struct timespec    now_tsp;
    clock_gettime(CLOCK_MONOTONIC, &now_tsp);
    return now_tsp.tv_sec;
}

u32 clock_realtime(void)
{
    struct timespec    now_tsp;
    clock_gettime(CLOCK_REALTIME, &now_tsp);
    return now_tsp.tv_sec;
}

u64 rand64(void)
{
    u64 k1;
    u64 k2;
    u64 k3;

    k1 = clock_monotonic_ns();
    k1 *= (k1 >> 32)^(k1 & 0xFFFFFFFF);
    k1 ^= (k1 & 0xFFFFFFFF) << 32;
    k2 = clock_monotonic_ns();
    k2 *= (k2 >> 32)^(k2 & 0xFFFFFFFF);
    k2 ^= (k2 & 0xFFFFFFFF) << 32;
    k3 = clock_monotonic_ns();
    k3 *= (k3 >> 32)^(k3 & 0xFFFFFFFF);
    k3 ^= (k3 & 0xFFFFFFFF) << 32;

    return (k1 * k2 * k3) ^ (k1 + k2 + k3) ^ (k1 - k2 - k3) ^ k1 ^ k2 ^ k3;
}

static ssize_t http_download_callback(void *buffer, size_t size, size_t nmemb, void *stream)
{
    return fwrite(buffer, size, nmemb, stream);
}

bool http_download(const char * file_path,const char * url)
{
    CURL *curl;
    CURLcode res;
    //struct curl_slist *headerlist = NULL;
    FILE * fp;
    //char user_cmd [64];
    //char pass_cmd [64];
    bool retval = false;

    fp = fopen(file_path, "wb");
    if(!fp) {
        return false; /* failure, can't open file to write */
    }

    curl = curl_easy_init();
    if(curl) {
        curl_easy_setopt(curl, CURLOPT_URL,url);//下载指定的文件
        /* 定义回调函数，以便在需要写入数据时进行调用 */
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, http_download_callback);
        /*设置一个指向我们的结构的指针传递给回调函数*/
        curl_easy_setopt(curl, CURLOPT_WRITEDATA,fp);

        /* 打开完整的协议/调试输出*/
        //curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

        res = curl_easy_perform(curl);
        if(CURLE_OK == res) {
            retval = true;
        }
        /* 释放所有curl资源*/
        curl_easy_cleanup(curl);
    }
    fclose(fp); /* 关闭本地文件 */
    return retval;
}

