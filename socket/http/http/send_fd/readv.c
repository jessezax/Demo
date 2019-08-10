#include "func.h"

int main()
{
	int fd;
	fd=open("file",O_RDWR);
	if(-1==fd)
	{
		perror("open");
		return -1;
	}
	struct iovec iov[2];
	bzero(iov,sizeof(iov));
	char buf1[10]={0};
	char buf2[10]={0};
	iov[0].iov_base=buf1;
	iov[0].iov_len=6;	
	iov[1].iov_base=buf2;
	iov[1].iov_len=4;	
	int ret;
	ret=readv(fd,iov,2);
	printf("ret=%d\n",ret);
	printf("buf1=%s,buf2=%s\n",buf1,buf2);
	close(fd);
	return 0;
}
