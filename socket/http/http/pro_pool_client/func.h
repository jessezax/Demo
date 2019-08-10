#include <strings.h>
#include <sys/uio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <netdb.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/epoll.h>

#define FILENAME "file"
#define BUFSIZE 10000
typedef struct{
	pid_t pid;//子进程的pid
	int tfds;//通过该管道传递内核控制信息
	short busy;//标示进程是否忙碌
}child,*pchild;
typedef struct{
	int len;
	char buf[BUFSIZE];
}train,*ptrain;

int sendbuf(int sfd,char *buf,int);
int recvbuf(int sfd,char* buf);
