#include "func.h"

int main(int argc,char* argv[])
{
	if(argc!=4)
	{
		printf("Please INPUT IP PORT PROCESS_NUM\n");
		return -1;
	}
	int child_num=atoi(argv[3]);
	pchild p=(pchild)calloc(child_num,sizeof(child));
	make_child(p,child_num);//创建子进程并初始化所有的数据结构
	int sfd;
	sfd=socket(AF_INET,SOCK_STREAM,0);
	if(-1==sfd)
	{
		perror("socket");
		return -1;
	}
	struct sockaddr_in ser;
	memset(&ser,0,sizeof(ser));
	ser.sin_family=AF_INET;
	ser.sin_port=htons(atoi(argv[2]));
	ser.sin_addr.s_addr=inet_addr(argv[1]);
	int ret;
	ret=bind(sfd,(struct sockaddr*)&ser,sizeof(ser));
	if(-1==ret)
	{
		perror("bind");
		return -1;
	}
	int epfd=epoll_create(1);
	struct epoll_event event,*evs;
	evs=(struct epoll_event*)calloc(child_num+1,sizeof(struct epoll_event));
	memset(&event,0,sizeof(event));
	event.events=EPOLLIN;
	event.data.fd=sfd;
	epoll_ctl(epfd,EPOLL_CTL_ADD,sfd,&event);
	int i;
	for(i=0;i<child_num;i++)//父进程监控每一个子进程管道的对端的可读事件
	{
		event.events=EPOLLIN;
		event.data.fd=p[i].tfds;
		epoll_ctl(epfd,EPOLL_CTL_ADD,p[i].tfds,&event);
	}
	listen(sfd,child_num);//端口就打开了
	int ret1;
	int new_fd;
	int j;
	char flag;
	while(1)
	{
		ret1=epoll_wait(epfd,evs,child_num+1,-1);
		for(i=0;i<ret1;i++)
		{
			if(evs[i].data.fd==sfd)//网络请求到达
			{
				new_fd=accept(sfd,NULL,NULL);
				//找到非忙碌的子进程，然后把new_fd发送给它
				for(j=0;j<child_num;j++)
				{	
					if(p[j].busy==0)
					{	
						send_fd(p[j].tfds,new_fd);//把描述符发送给子进程
						p[j].busy=1;
						printf("find a not busy process,send success\n");
						break;
					}
				}
				close(new_fd);//父进程close对应的new_fd,将new_fd引用计数
			}
			for(j=0;j<child_num;j++)
			{
				if(evs[i].data.fd==p[j].tfds)
				{
					read(p[j].tfds,&flag,sizeof(flag));//把数据读出来
					p[j].busy=0;
					printf("child is not busy\n");
				}
			}	
		}
	}
}
