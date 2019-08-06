#include<stdio.h>

void add(int *a){
  int cons = *a;
  cons++;
  printf("cons:%d\n",cons);
}

int main(){
  int b=1;
  add(&b);
  printf("%d\n",b);

}
