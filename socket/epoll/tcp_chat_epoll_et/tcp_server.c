#include "func.h"

void sig(int signum)
{
	printf("signal %d is coming\n",signum);
}
void set_status(int fd)
{
	int status=fcntl(fd,F_GETFL);
	status=status|O_NONBLOCK;
	fcntl(fd,F_SETFL,status);
}
int main(int argc,char* argv[])
{
	if(argc!=3)
	{
		printf("error args\n");
		return -1;
	}
	signal(SIGPIPE,sig);
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
	listen(sfd,10);//端口就打开了
	int new_fd=-1;
	struct sockaddr_in cli;
	int len=sizeof(cli);
	char buf[10]={0};
	int epfd=epoll_create(1);
	struct epoll_event event,evs[3];
	memset(&event,0,sizeof(event));
	event.data.fd=STDIN_FILENO;
	event.events=EPOLLIN;
	ret=epoll_ctl(epfd,EPOLL_CTL_ADD,STDIN_FILENO,&event);//注册标准输入
	if(-1==ret)
	{
		perror("epoll_ctl");
		return -1;
	}
	event.data.fd=sfd;
	event.events=EPOLLIN;
	ret=epoll_ctl(epfd,EPOLL_CTL_ADD,sfd,&event);//注册sfd
	if(-1==ret)
	{
		perror("epoll_ctl");
		return -1;
	}
	int i;
	int ret1;
	while(1)
	{
		memset(evs,0,sizeof(evs));//每次用evs前清空
		ret1=epoll_wait(epfd,evs,3,-1);
		for(i=0;i<ret1;i++)
		{
			if(evs[i].data.fd==new_fd)
			{
				while(1)
				{
					memset(buf,0,sizeof(buf));
					ret=recv(new_fd,buf,sizeof(buf)-1,0);//recv会阻塞
					if(ret>0)
					{
						printf("%s",buf);
					}else if(0==ret)
					{
						printf("byebye\n");
						close(new_fd);
						event.data.fd=new_fd;
						event.events=EPOLLIN;
						epoll_ctl(epfd,EPOLL_CTL_DEL,new_fd,&event);//解注册new_fd
						break;
					}else if(errno==EAGAIN){
						printf("\n");
						break;
					}
				}
			}
			if(evs[i].data.fd==sfd)
			{
				new_fd=accept(sfd,(struct sockaddr*)&cli,&len);
				if(-1==new_fd)
				{
					perror("accept");
					return -1;
				}
				event.data.fd=new_fd;
				event.events=EPOLLIN|EPOLLET;//改为边沿触发
				epoll_ctl(epfd,EPOLL_CTL_ADD,new_fd,&event);//注册new_fd
				set_status(new_fd);//改为非阻塞				
				printf("client addr=%s,client port=%d\n",inet_ntoa(cli.sin_addr),ntohs(cli.sin_port));
			}
			if(evs[i].data.fd==0)
			{
				memset(buf,0,sizeof(buf));
				ret=read(0,buf,sizeof(buf));
				if(ret>0)
				{
					ret=send(new_fd,buf,strlen(buf)-1,0);
					if(ret<=0)
					{
						printf("send ret=%d\n",ret);
						perror("send");
					}
				}else{
					printf("byebye\n");
					break;
				}
			}
		}
	}
	close(new_fd);
	close(sfd);
	return 0;
}
	
