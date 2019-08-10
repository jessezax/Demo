#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
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





#define PORT 80			//目标地址端口号
#define ADDR "39.156.69.79" //目标地址IP

int main()
{
  int iSocketFD = 0; //socket句柄
  unsigned int iRemoteAddr = 0;
  struct sockaddr_in stRemoteAddr = {0} ; //对端，即目标地址信息
  socklen_t socklen = 0;  	
  char buf[4096] = {0}; //存储接收到的数据

  iSocketFD = socket(AF_INET, SOCK_STREAM, 0); //建立socket
  if(0 > iSocketFD)
  {
    printf("创建socket失败！\n");
    return 0;
  }	

  stRemoteAddr.sin_family = AF_INET;
  stRemoteAddr.sin_port = htons(PORT);
  inet_pton(AF_INET, ADDR, &iRemoteAddr);
  stRemoteAddr.sin_addr.s_addr=iRemoteAddr;

  //连接方法： 传入句柄，目标地址，和大小
  if(0 > connect(iSocketFD, (void *)&stRemoteAddr, sizeof(stRemoteAddr)))
  {
    printf("连接失败！\n");
  }else{
    printf("connect success\n");
    char req[1000] = "GET /index.html HTTP/1.1\nHost: 39.156.69.79\nConnection: keep-alive\n\n\n\r\n\r\n";

    if(0 > write(iSocketFD,req,strlen(req)+1))
    printf("send");
    memset(buf,0,sizeof(buf));
    read(iSocketFD, buf, sizeof(buf)); ////将接收数据打入buf，参数分别是句柄，储存处，最大长度，其他信息（设为0即可）。?
    printf("Received:%s\n", buf);
  }

  close(iSocketFD);//关闭socket	
  return 0;

}
