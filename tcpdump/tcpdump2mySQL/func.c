#ifndef _HEAD_H
#define _HEAD_H
#include "head.h"


void add_record(MYSQL *pConn,tuples quintuples)
{
  char cmd[1024] = {0};
  int ret = 0;
  //tuples quintuples={"192.168.0.1","1","192.168.80.0.2","2","TCP"};
  // 强大的sprintf为用户减少输入 ，避免让用户输入完整的命令，而只需要记录的数据输入
   sprintf(cmd, "insert into quintuple(SourceIp,SourcePort,DestinIp,DestinPort,protocol) values('%s','%d','%s','%d','%d')",quintuples.SourceIp,quintuples.SourcePort,quintuples.DestinIp,quintuples.DestinPort,quintuples.protocol);
   ret = mysql_real_query(pConn, cmd, (unsigned int)strlen(cmd));
   if(! ret) // ret = 0是表示正确
   {
   // 打印受到影响的行的个数
   printf("Inserted by %lu rows\n",(unsigned long)mysql_affected_rows(pConn));
   }
   else // ret != 0 表示出错
   {
   // 打印出错及相关信息
   fprintf(stderr, "Insert error %d:%s\n", mysql_errno(pConn), mysql_error(pConn));
   }
   return ;
}

void display_record(MYSQL* pConn)
{
  int ret = 0;
  MYSQL_RES *pRes=NULL;
  int numFields =0;
  int i =0;
  MYSQL_ROW Row=NULL;
  char cmd[1024] = {0};
  sprintf(cmd,"select* from quintuple"); // 整张表查找
  ret = mysql_real_query(pConn, cmd, (unsigned int)strlen(cmd));
  if(ret) // 出错
  {
    printf("select error:%s\n", mysql_error(pConn));

  }
  else
  {
    pRes = mysql_store_result(pConn);
    if(pRes)
    {
      printf("一共%d行\n",(int)mysql_num_rows(pRes));
      numFields = mysql_num_fields(pRes); // 获取列的个数
      while(Row = mysql_fetch_row(pRes)) // 取出每条记录
      {
        printf("id        SourceIp      SourcePort    DestinIp     DestinPort     protocol\n");
        for(i = 0; i < numFields; i++)
        {
          printf("%s        ",Row[i]?Row[i]:NULL);
        }
        printf("\n");
      }
      if(mysql_errno(pConn))
      {
        fprintf(stderr, "Retrieve error:%s\n", mysql_error(pConn));
      }
    }
  }
   mysql_free_result(pRes);
  return;
}




void mod_record(MYSQL *pConn,int id)
{
  int ret = 0;
  MYSQL_RES *pRes=NULL;
  char cmd[1024] = {0};
  tuples quintuples={"192.168.0.1",1,"192.168.80.0.2",2,4};
  sprintf(cmd,"update quintuple  set SourceIp='%s',SourcePort='%d',DestinIp='%s',DestinPort='%d',protocol='%d' where id=%d ",quintuples.SourceIp,quintuples.SourcePort,quintuples.DestinIp,quintuples.DestinPort,quintuples.protocol,id);
    ret = mysql_real_query(pConn, cmd, (unsigned int)strlen(cmd));
  if(! ret) // ret = 0是表示正确
  {
    // 打印受到影响的行的个数
     printf("Updated by %lu rows\n",(unsigned long)mysql_affected_rows(pConn) );
    //
  }
  else
  {
    // 打印出错及相关信息
     fprintf(stderr, "Updated error %d:%s\n", mysql_errno(pConn),
     mysql_error(pConn));
  }
   mysql_free_result(pRes);
  // return ;
}









#endif
