#include "func.h"

void child_handle(int fdr)
{
  char flag=1;
  int new_fd;
  char buf[BUFSIZE];
  memset(buf,0,sizeof(buf));
  recv_fd(fdr,&new_fd);//从父进程接收任务
  while(1)
  {
    memset(buf,0,sizeof(buf));

    recvbuf(new_fd,buf);
    //	send_data(new_fd);
    write(fdr,&flag,sizeof(flag));//通知父进程，完成发送任务
    printf("buf is %s\n",buf);
    get_http_buf(new_fd,buf);
  }
}

void make_child(pchild p,int num)
{
  int i;
  int fds[2];
  pid_t pid;
  for(i=0;i<num;i++)//创建多个子进程
  {
    socketpair(AF_LOCAL,SOCK_STREAM,0,fds);
    pid=fork();
    if(0==pid)
    {
      close(fds[1]);
      child_handle(fds[0]);
    }//让子进程永远不能从这个括号走出来
    close(fds[0]);
    p[i].pid=pid;//子进程的pid
    p[i].tfds=fds[1];//拿到管道的一端
    p[i].busy=0;//子进程处于空闲状态
  }
}
