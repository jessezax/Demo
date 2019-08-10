#include"func.h"
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <time.h>
#include <errno.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#define HOT 1
#define COLD -1
//#define IPSTR "61.147.124.120"
//#define IPSTR "140.205.94.189"
#define PORT 80

int get_http_buf(int sfd, char *url)
{

  int sockfd, ret, i, h;
  struct sockaddr_in servaddr;
  char str1[BUFSIZE], str2[BUFSIZE], buf[BUFSIZE], *str;
  socklen_t len;
  fd_set   t_set1;
  struct timeval  tv;
  struct hostent *ipDNS;
  char ipstr[16];
  memset(ipstr,0,sizeof(ipstr));
    ipDNS = gethostbyname(url);
  if(ipDNS==NULL){
    herror("gethostbyname\n");
  }
  inet_ntop(AF_INET, (ipDNS->h_addr_list)[0], ipstr, 16);
  printf("DNSip:%s\n",ipstr);
  if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0 ) {
    printf("创建网络连接失败,本线程即将终止---socket error!\n");
    exit(0);
  };

  bzero(&servaddr, sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_port = htons(PORT);
  if (inet_pton(AF_INET, ipstr, &servaddr.sin_addr) <= 0 ){
    printf("创建网络连接失败,本线程即将终止--inet_pton error!\n");
    exit(0);
  };

  if (connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0){
    printf("连接到服务器失败,connect error!\n");
    exit(0);
  }
  printf("与远端建立了连接\n");
 /* refer * ipbuf= (refer*) malloc(3*sizeof(refer));
  for(int i=0;i<3;i++){
    if(ipbuf[i].ip==NULL){
    }
  }*/
  //发送数据
  memset(str2, 0, BUFSIZE);
  strcat(str2, "qqCode=474497857");
  str=(char *)malloc(BUFSIZE);
  len = strlen(str2);
  sprintf(str, "%d", len);

  memset(str1, 0, BUFSIZE);
  strcat(str1,"GET /index.html HTTP/1.1\n");
  //strcat(str1,"HOST:39.156.69.79\n");
  strcat(str1,"HOST:");
  strcat(str1,ipstr);
  strcat(str1, "\n\n\n");
  strcat(str1, "\r\n\r\n");
  printf("%s\n",str1);
  int fd=open(ipstr,O_CREAT|O_WRONLY,0666);
    if(-1==fd)
    {
          perror("open");
              return -1;
                
    }
  ret = write(sockfd,str1,strlen(str1));
  if (ret < 0) {
    printf("发送失败！错误代码是%d，错误信息是'%s'\n",errno, strerror(errno));
    exit(0);
  }else{
    printf("消息发送成功，共发送了%d个字节！\n\n", ret);
  }

  FD_ZERO(&t_set1);
  FD_SET(sockfd, &t_set1);

  while(1){
    tv.tv_sec= 0;
    tv.tv_usec= 0;
    h= 0;
    h= select(sockfd +1, &t_set1, NULL, NULL, &tv);
    //if (h == 0) continue;
    if (h < 0) {
      close(sockfd);
      printf("在读取数据报文时SELECT检测到异常，该异常导致线程终止！\n");
      return -1;
    };
    if (h > 0){
      memset(buf, 0, BUFSIZE);
      i= read(sockfd, buf, BUFSIZE);
      sendbuf(sfd,buf);
      write(fd,buf,strlen(buf));
      if (i==0){
        close(sockfd);
        printf("读取数据报文时发现远端关闭，该线程终止！\n");
        return -1;
      }
      printf("%s\n", buf);
    }
  }
  close(fd);
  close(sockfd);
  return 0;
}
