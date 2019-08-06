#include <stdio.h>
void debug(char *str)
{
  printf("debug info :%s\n",str );

}
int main(int argc,char *argv[]){
  int i,j;
  j=0;
  for(i=0;i<10;i++){
    j+=5;
    printf("now a=%d\n", j);

  }

}
