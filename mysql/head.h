#ifndef _FUNC_H
#define _FUNC_H
#include <stdio.h>
#include <stdlib.h>
#include </usr/include/mysql/mysql.h>
#include <string.h>

void add_record(MYSQL *pConn);
void display_record(MYSQL* pConn);
void mod_record(MYSQL *pConn,int id);
  
typedef struct quintuples{   
  char SourceIp[128] ;
  char SourcePort[10] ;
  char DestinIp[128] ;
  char DestinPort[10] ;
  char protocol[10] ;
}tuples;

#endif
