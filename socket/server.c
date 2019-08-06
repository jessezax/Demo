#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
 
#define PORT 23		//�˿ں�
#define BACKLOG 5	//��������
 
int main()
{
	int iSocketFD = 0;  //socket���
	int iRecvLen = 0;   //���ճɹ���ķ���ֵ
	int new_fd = 0; 	//�������Ӻ�ľ��
	char buf[4096] = {0}; //
	struct sockaddr_in stLocalAddr = {0}; //���ص�ַ��Ϣ�ṹͼ�������о�������Ը�ֵ
	struct sockaddr_in stRemoteAddr = {0}; //�Է���ַ��Ϣ
	socklen_t socklen = 0;  	
 
	iSocketFD = socket(AF_INET, SOCK_STREAM, 0); //����socket
	if(0 > iSocketFD)
	{
		printf("����socketʧ�ܣ�\n");
		return 0;
	}	
 
	stLocalAddr.sin_family = AF_INET;  /*�����Ա�ʾ���ձ�����������������*/
	stLocalAddr.sin_port = htons(PORT); /*�˿ں�*/
	stLocalAddr.sin_addr.s_addr=htonl(INADDR_ANY); /*IP���������ݱ�ʾ����IP*/
 
	//�󶨵�ַ�ṹ���socket
	if(0 > bind(iSocketFD, (void *)&stLocalAddr, sizeof(stLocalAddr)))
	{
		printf("��ʧ�ܣ�\n");
		return 0;
	}
 
	//�������� ���ڶ�����������������
	if(0 > listen(iSocketFD, BACKLOG))
	{
		printf("����ʧ�ܣ�\n");
		return 0;
	}
 
	printf("iSocketFD: %d\n", iSocketFD);	
	//����������֪�����յ���Ϣ�������ֱ���socket��������յ��ĵ�ַ��Ϣ�Լ���С?
	new_fd = accept(iSocketFD, (void *)&stRemoteAddr, &socklen);
	if(0 > new_fd)
	{
		printf("accept failed��\n");
		return 0;
	}else{
		printf("accept success\n");
		//�������ݣ������ֱ������Ӿ�������ݣ���С��������Ϣ����Ϊ0���ɣ�?
		send(new_fd, "hello", sizeof("hello"), 0);
	}
 
	printf("new_fd: %d\n", new_fd);	
	iRecvLen = recv(new_fd, buf, sizeof(buf), 0);	
	if(0 >= iRecvLen)    //�Զ˹ر����� ����0
	{	
		printf("close connect\n");
	}else{
		printf("buf: %s\n", buf);
	}
 
	close(new_fd);
	close(iSocketFD);
 
	return 0;
}
