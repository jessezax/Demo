#include "func.h"

int main()
{
	int fd;
	fd=open("file",O_CREAT|O_RDWR,0666);
	if(-1==fd)
	{
		perror("open");
		return -1;
	}
	struct iovec iov[2];
	bzero(iov,sizeof(iov));
	char buf1[10]="hello";
	char buf2[10]="world";
	iov[0].iov_base=buf1;
	iov[0].iov_len=5;	
	iov[1].iov_base=buf2;
	iov[1].iov_len=5;	
	int ret;
	ret=writev(fd,iov,2);
	printf("ret=%d\n",ret);
	close(fd);
	return 0;
}
