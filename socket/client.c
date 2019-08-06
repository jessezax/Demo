
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
 
#define PORT 23			//Ŀ���ַ�˿ں�
#define ADDR "192.168.245.140" //Ŀ���ַIP
 
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
		//printf("connect failed:%d",errno);//ʧ��ʱҲ�ɴ�ӡerrno
	}else{
		printf("connect success��\n");
		recv(iSocketFD, buf, sizeof(buf), 0); ////���������ݴ���buf�������ֱ��Ǿ�������洦����󳤶ȣ�������Ϣ����Ϊ0���ɣ���?
		printf("Received:%s\n", buf);
	}
	
	close(iSocketFD);//�ر�socket	
	return 0;

}
