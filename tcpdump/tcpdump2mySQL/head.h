#ifndef _FUNC_H
#define _FUNC_H
#include <stdio.h>
#include <stdlib.h>
#include </usr/include/mysql/mysql.h>
#include <string.h>

typedef struct quintuples{   
  char SourceIp[128] ;
  int SourcePort ;
  char DestinIp[128] ;
  int DestinPort ;
  u_char protocol ;
}tuples;

void add_record(MYSQL *pConn,tuples );
void display_record(MYSQL* pConn);
void mod_record(MYSQL *pConn,int id);
  

#endif
