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





#define PORT 80			//Ŀ���ַ�˿ں�
#define ADDR "39.156.69.79" //Ŀ���ַIP

int main()
{
  int iSocketFD = 0; //socket���
  unsigned int iRemoteAddr = 0;
  struct sockaddr_in stRemoteAddr = {0} ; //�Զˣ���Ŀ���ַ��Ϣ
  socklen_t socklen = 0;  	
  char buf[4096] = {0}; //�洢���յ�������

  iSocketFD = socket(AF_INET, SOCK_STREAM, 0); //����socket
  if(0 > iSocketFD)
  {
    printf("����socketʧ�ܣ�\n");
    return 0;
  }	

  stRemoteAddr.sin_family = AF_INET;
  stRemoteAddr.sin_port = htons(PORT);
  inet_pton(AF_INET, ADDR, &iRemoteAddr);
  stRemoteAddr.sin_addr.s_addr=iRemoteAddr;

  //���ӷ����� ��������Ŀ���ַ���ʹ�С
  if(0 > connect(iSocketFD, (void *)&stRemoteAddr, sizeof(stRemoteAddr)))
  {
    printf("����ʧ�ܣ�\n");
  }else{
    printf("connect success\n");
    char req[1000] = "GET /index.html HTTP/1.1\nHost: 39.156.69.79\nConnection: keep-alive\n\n\n\r\n\r\n";

    if(0 > write(iSocketFD,req,strlen(req)+1))
    printf("send");
    memset(buf,0,sizeof(buf));
    read(iSocketFD, buf, sizeof(buf)); ////���������ݴ���buf�������ֱ��Ǿ�������洦����󳤶ȣ�������Ϣ����Ϊ0���ɣ���?
    printf("Received:%s\n", buf);
  }

  close(iSocketFD);//�ر�socket	
  return 0;

}
