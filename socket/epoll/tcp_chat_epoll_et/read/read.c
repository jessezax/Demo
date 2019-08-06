#include "func.h"

void set_status(int fd)
{
	int status=fcntl(fd,F_GETFL);
	status=status|O_NONBLOCK;
	fcntl(fd,F_SETFL,status);
}


int main()
{
	char buf[128]={0};
	int ret;
	set_status(0);
	ret=read(0,buf,sizeof(buf));
	printf("ret=%d,buf=%s,errno=%d\n",ret,buf,errno);
	return 0;
}
