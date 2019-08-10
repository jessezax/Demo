#include "func.h"

void send_data(int new_fd)
{
	train t;
	memset(&t,0,sizeof(t));
	strcpy(t.buf,FILENAME);
	t.len=strlen(t.buf);
	//发送文件名给对客户端
	int ret;
	ret=send_n(new_fd,&t,4+t.len);
	if(-1==ret)
	{
		perror("send");
		return;
	}
	int fd;
	fd=open(FILENAME,O_RDONLY);
	if(-1==fd)
	{
		perror("open");
		return;
	}
	//开多趟火车，发文件内容
	while(memset(&t,0,sizeof(t)),(t.len=read(fd,t.buf,sizeof(t.buf)))>0)
	{
		send_n(new_fd,&t,4+t.len);
	}
	t.len=0;
	//发送空火车，标示文件已经发送结束
	send_n(new_fd,&t,4+t.len);	
	close(new_fd);
}
