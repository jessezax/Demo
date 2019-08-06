#ifndef _FUNC_C
#define _FUNC_C
#include "head.h"
#endif

int main()
{
  MYSQL*mysql=mysql_init(0);//创建一个连接对象
  if (!mysql_real_connect(mysql,"localhost","root","","traffic",0,0,0))//密码为空；数据库：traffic；连接失败时FALSE
    {
      printf("无法连接数据库:%s\n",mysql_error(mysql));
      exit(-1);
    }
  else
  {
    printf("连接数据库成功\n");
 //   add_record(mysql);
  display_record(mysql);
  mod_record(mysql,1);
  display_record(mysql);
  }
  return 0;
}
