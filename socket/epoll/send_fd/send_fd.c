#include "func.h"

void send_fd(int fdw,int fd)
{
	struct msghdr msg;
	memset(&msg,0,sizeof(msg));
	struct cmsghdr *cmsg;
	int len=CMSG_LEN(sizeof(int));
	cmsg=(struct cmsghdr *)calloc(1,len);
	cmsg->cmsg_len=len;
	cmsg->cmsg_level = SOL_SOCKET;
    cmsg->cmsg_type = SCM_RIGHTS;
	*(int*)CMSG_DATA(cmsg)=fd;
	msg.msg_control=cmsg;
	msg.msg_controllen=len;
	char buf1[10]="hello";
	char buf2[10]="world";
	struct iovec iov[2];
	iov[0].iov_base=buf1;
	iov[0].iov_len=5;
	iov[1].iov_base=buf2;
	iov[1].iov_len=5;
	msg.msg_iov=iov;
	msg.msg_iovlen=2;
	int ret=sendmsg(fdw,&msg,0);
	if(-1==ret)
	{
		perror("sendmsg");
		return;
	}
}
void recv_fd(int fdr,int* fd)
{
	struct msghdr msg;
	memset(&msg,0,sizeof(msg));
	struct cmsghdr *cmsg;
	int len=CMSG_LEN(sizeof(int));
	cmsg=(struct cmsghdr *)calloc(1,len);
	cmsg->cmsg_len=len;
	cmsg->cmsg_level = SOL_SOCKET;
    cmsg->cmsg_type = SCM_RIGHTS;
	msg.msg_control=cmsg;
	msg.msg_controllen=len;
	char buf1[10]="hello";
	char buf2[10]="world";
	struct iovec iov[2];
	iov[0].iov_base=buf1;
	iov[0].iov_len=5;
	iov[1].iov_base=buf2;
	iov[1].iov_len=5;
	msg.msg_iov=iov;
	msg.msg_iovlen=2;
	int ret=recvmsg(fdr,&msg,0);
	if(-1==ret)
	{
		perror("sendmsg");
		return;
	}
	*fd=*(int*)CMSG_DATA(cmsg);
}


int main()
{
	int fds[2];
	socketpair(AF_LOCAL,SOCK_STREAM,0,fds);	
	if(!fork())
	{
		close(fds[1]);
		int fd;
		recv_fd(fds[0],&fd);
		printf("I am child,%d\n",fd);
		char buf[10]={0};
		read(fd,buf,sizeof(buf));
		printf("read success,%s\n",buf);
		exit(0);
	}else{
		close(fds[0]);
		int fd;
		fd=open("file",O_RDWR);
		printf("parent process fd=%d\n",fd);
		send_fd(fds[1],fd);	
		wait(NULL);
	}
}
