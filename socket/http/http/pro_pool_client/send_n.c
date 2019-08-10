#include "func.h"

int send_n(int sfd,char* p,int len)
{
  int total=0;
  int ret;
  while(total<len)
  {
    ret=send(sfd,p+total,len-total,0);
    total=total+ret;
  }
  return 0;
}

int recv_n(int sfd,char* p,int len)
{
  int total=0;
  int ret;
  while(total<len)
  {
    ret=recv(sfd,p+total,len-total,0);
    total=total+ret;
  }
  return 0;
}


int sendbuf(int sfd,char *buf,int buflen){
  train t;
  memset(&t,0,sizeof(t));
  strcpy(t.buf,buf); 
  //t.len=strlen(t.buf);
  t.len=buflen;
  int ret;            
  ret=send_n(sfd,&t,4+t.len);
  if(-1==ret)                   
  {          
    perror("send");
    return -1;        
  }
  return ret;
}

int recvbuf(int sfd,char* buf){
    int len;
    int ret;
    ret = recv_n(sfd,&len,sizeof(len));
    recv_n(sfd,buf,len);  
    printf("recv %s\n",buf);
  if(-1==ret)                   
  {          
    perror("recv error");
    return -1;        
  }
  return ret;
}



