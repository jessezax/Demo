#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
int main(int argc, char **argv)
{

  struct hostent *answer;
  int i;
  char ipstr[16];

  answer = gethostbyname("baidu.com");
  if (answer == NULL) {
    herror("gethostbyname"); //由gethostbyname自带的错误处理函数
    exit(1);

  }

  for (i = 0; (answer->h_addr_list)[i] != NULL; i++) {
    inet_ntop(AF_INET, (answer->h_addr_list)[i], ipstr, 16);
    printf("%s\n", ipstr);
    printf("officail name : %s\n", answer->h_name);

 }
  exit(0);

}
