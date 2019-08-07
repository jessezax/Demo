/**
 * \file plc_send.c
 * \version 1.0
 * \date 2017/10/18
 * \brief 按照定义的模板格式构造二进制数据发送
 *
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>

#include <time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <endian.h>
#include <stdbool.h>
#include <openssl/des.h>

#define VERSION   "1.0.0.1"

static unsigned char *keystring = "12345678";

typedef struct st_data_buff
{
	char *buff;
	char *pdatalen;
	int buff_len;
	int offset;
	int line_num;
}t_data_buff;

typedef struct st_sync_ipport
{
	char ipstr[64];
	unsigned short port;
	int sockfd;
}t_sync_ipport;

typedef struct st_dir_param
{
	int sync_ipport_count;
	int sync_ipport_capacity;
	t_sync_ipport *sync_ipport;
	t_data_buff file_info;
}t_dir_param;

/**
 * 读取文件每一行之后,执行的回调函数指针
 *
 */
typedef int (*FILE_LINE_CALLBACK)(void *result,char *linebuf,int linelen);

/**
 * 遍历目录之后,对目录下文件进行处理的回调函数指针
 */
typedef int (*DIR_FILE_CALLBACK)(void *userparam,char *filename);

/**
 * 字段打印的处理函数指针
 *
 */
typedef int (*FILED_OUTPUT_CALLBACK) (t_data_buff *data_buff,void *indata, int len);


#define rt_ntohll(x) (((x) >> 56) |\
					(((x) & 0x00ff000000000000) >> 40) |\
					(((x) & 0x0000ff0000000000) >> 24) |\
					(((x) & 0x000000ff00000000) >> 8)    |\
					(((x) & 0x00000000ff000000) << 8)    |\
					(((x) & 0x0000000000ff0000) << 24) |\
					(((x) & 0x000000000000ff00) << 40) |\
					(((x) << 56)))

/**
 * 功能:去掉字符串左右空格
 * @param [in] 需要去掉左右空格的字符串
 *
 */
static void str_all_trim(char *str)
{
	char *head = NULL;
	char *tail = NULL;

	if(str == NULL)
		return;
	for(head = str; *head == ' ' || *head == '\t'; head++);

	for(tail = str + strlen(str) -1; (*tail == ' ' || *tail == '\t') && tail >=head; tail--);

	strncpy(str, head,tail - head + 1);
	str[tail - head + 1] = '\0';
}

static time_t str_timestamp_to_unix(char *src)
{
	int year, mon,day, hour,min,sec;
	time_t stamp;
	struct tm when;

	if (6 != sscanf(src, "%d-%d-%d %d:%d:%d", &year, &mon, &day, &hour, &min, &sec))
	{
		fprintf(stderr,"[%s] timestap val may be error!\n",__FUNCTION__);
		return 0;
	}

	when.tm_year = year - 1900;
	when.tm_mon  = mon - 1;
	when.tm_mday = day;
	when.tm_hour = hour;
	when.tm_min  = min;
	when.tm_sec  = sec;

	stamp = mktime(&when);
	return stamp;
}

static unsigned long htonl64(unsigned long host)
{
	unsigned long ret = 0;

	// #ifdef __BYTE_ORDER==__LITTLE_ENDIAN //dai
	#if __BYTE_ORDER==__LITTLE_ENDIAN  //dai

	unsigned int high, low;

	low  = (unsigned int)(host & 0x00000000FFFFFFFFLL);
	high = (unsigned int)((host & 0xFFFFFFFF00000000LL) >> 32);

	low  = htonl(low);
	high = htonl(high);

	ret = (unsigned long) high + (((unsigned long) low) << 32);
	#else

	ret = host;
	#endif

	return ret;
}

/*************************** 策略发送 ****************************/
/**
 * 功能: 创建ipv4 tcp连接
 * \param [in] ipv4, ipv4地址,网络字节序
 * \param [in] port, 端口 网络字节序
 *
 */
static int eu_create_ipv4_tcp_conn(unsigned int ipv4,unsigned short port)
{
	int sockfd;
	//sockfd = socket(PF_INET,SOCK_STREAM,IPPROTO_TCP);
	sockfd = socket(AF_INET,SOCK_STREAM, 0);
	if(sockfd == -1 )
	{
		return -1;
	}

	int sockopt = 1;

	struct timeval tm;
	tm.tv_sec  = 10;
	tm.tv_usec = 0;

	setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (struct timeval *)&tm, sizeof(tm));
	setsockopt(sockfd, SOL_SOCKET, SO_SNDTIMEO, (struct timeval *)&tm, sizeof(tm));
	setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR,(char *)&sockopt,sizeof(sockopt));

	struct sockaddr_in sockaddr;
	memset(&sockaddr,0,sizeof(struct sockaddr));
	sockaddr.sin_family         = AF_INET;
	sockaddr.sin_addr.s_addr    = ipv4;
	sockaddr.sin_port           = port;
	if(connect(sockfd,(struct sockaddr *)(&sockaddr),sizeof(struct sockaddr)) == -1)
	{
		fprintf(stderr,"[%s] connect to cu socket server error.%s!\n", __FUNCTION__,strerror(errno));
		close(sockfd);
		//LOG(NULL,L_ERR,"[%s] connect to cu socket server error.%s!\n", __FUNCTION__,strerror(errno));
		return -1;
	}

	return sockfd;
}

/*************************** 策略发送 ****************************/
/**
 * 功能: 创建ipv6 tcp连接
 * \param [in] ipv6, ipv6地址
 * \param [in] port, 端口 网络字节序
 *
 */
static int eu_create_ipv6_tcp_conn(void *ipv6, unsigned short port)
{
	int sockfd;
	struct sockaddr_in6 sockaddr;

	sockfd = socket(AF_INET6, SOCK_STREAM, 0);
	if(-1 == sockfd){
		return -1;
	}

	int sockopt = 1;
	struct timeval tm;
	tm.tv_sec  = 10;
	tm.tv_usec = 0;

	setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (struct timeval *)&tm, sizeof(tm));
	setsockopt(sockfd, SOL_SOCKET, SO_SNDTIMEO, (struct timeval *)&tm, sizeof(tm));
	setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR,(char *)&sockopt,sizeof(sockopt));

	memset(&sockaddr, 0x0, sizeof(struct sockaddr_in6));
	sockaddr.sin6_family 		= AF_INET6;
	strncpy(sockaddr.sin6_addr.__in6_u.__u6_addr8, (char *)ipv6, strlen((char *)ipv6));
	sockaddr.sin6_port 			= port;
	if(connect(sockfd,(struct sockaddr *)(&sockaddr),sizeof(struct sockaddr_in6)) == -1)
	{
		fprintf(stderr,"[%s] connect to cu socket server error.%s!\n", __FUNCTION__,strerror(errno));
		close(sockfd);
		//LOG(NULL,L_ERR,"[%s] connect to cu socket server error.%s!\n", __FUNCTION__,strerror(errno));
		return -1;
	}

	return sockfd;

}

/**
 * 功能:创建tcp连接
 * \param [in] ipaddr Ip地址的地址, 网络字节序
 * \param [in] port 端口,网络字节序
 * \param [in] type ip地址类型, 4表示ipv4, 6 表示ipv6
 */
static inline int eu_create_tcp_conn(void *ipaddr,unsigned short port,unsigned char type)
{
	int sockfd = -1;
	unsigned int  ipv4;
	char buf[64] = {0};
	unsigned char *ipv6 = buf;

	if (type == 4)
	{
		ipv4    = *((unsigned int *)ipaddr);
		sockfd  = eu_create_ipv4_tcp_conn(ipv4,port);
		return sockfd;
	}
	else if(type == 6)
	{
		ipv6    = (unsigned char *)ipaddr;
		sockfd 	= eu_create_ipv6_tcp_conn(ipv6, port);
		return sockfd;
	}
	else
	{
		fprintf(stderr,"[%s] create tcp conn failure!\n",__FUNCTION__);
	}

	return sockfd;
}

typedef struct{
	unsigned int    version;
	unsigned int    msg_type;
	unsigned char   magic_number[4];
	unsigned int    device_ip;
	unsigned int    msg_id;
	unsigned int    msg_len;
	char            data[0];
}t_filter_msg_header;

static bool need_crypto(void *pbuff)
{
	t_filter_msg_header *msg = (t_filter_msg_header *)pbuff;
	if((msg->magic_number[0] == 0x6f) && (msg->magic_number[1] == 0x7f) && (msg->magic_number[2] == 0x8f) && (msg->magic_number[3] == 0x9f)){
		msg->msg_len = htonl(ntohl(msg->msg_len) - sizeof(t_filter_msg_header));
		return true;
	}else{
		return false;
	}
}

static int cal_msg_len(int size)
{
	return (size + 7) / 8 * 8;
}

static bool encrypt(void *input, void *output, int size)
{
	DES_cblock          key;
	DES_key_schedule    key_schedule;
	//生成一个 key
	DES_string_to_key(keystring, &key);
	if (DES_set_key_checked(&key, &key_schedule) != 0) {
		printf("convert to key_schedule failed.\n");
		return false;
	}
	//IV
	DES_cblock ivec;
	memset((char*)&ivec, 0, sizeof(ivec));
	DES_ncbc_encrypt(input, output, size, &key_schedule, &ivec, DES_ENCRYPT);
	return true;
}

static int plc_send_data(int sockfd, char *pbuff, int buff_len)
{
	int                 send_len        = 0;
	int                 cur_send_len    = 0;
	int                 send_limit      = 1000;
	int                 plc_buff_len    = buff_len;

	unsigned char out_buf[1024] 		= {0};
	unsigned char send_buf[1024]		= {0};
	unsigned char *output = out_buf;

	for(; send_len < plc_buff_len; send_len += cur_send_len)
	{
		if(need_crypto(pbuff + send_len)){
			int data_len = 0;
			t_filter_msg_header *msg = (t_filter_msg_header *)(pbuff+send_len);
			#if 0
			encrypt(msg, output, sizeof(t_filter_msg_header));
			encrypt(msg->data, output + cal_msg_len(sizeof(t_filter_msg_header)), ntohl(msg->msg_len));
			data_len = cal_msg_len(sizeof(t_filter_msg_header)) + cal_msg_len(ntohl(msg->msg_len));
			#endif

			t_filter_msg_header *send_msg = (t_filter_msg_header *)send_buf;
			memcpy(send_msg, msg, sizeof(t_filter_msg_header));
			encrypt(msg->data, send_msg->data, ntohl(msg->msg_len));
			data_len = sizeof(t_filter_msg_header) + strlen(send_msg->data);
			send_msg->msg_len = data_len;

			if(plc_buff_len - send_len < send_limit){
				send_limit = data_len;
			}
			if((cur_send_len = send(sockfd,send_msg,send_limit,0)) < 0)
				{
					close(sockfd);
					return -1;
				}
			}
		else{
			if(plc_buff_len - send_len < send_limit)
			{
				send_limit = plc_buff_len - send_len;
			}
			if((cur_send_len = send(sockfd,pbuff + send_len,send_limit,0)) < 0)
			{
				close(sockfd);
				return -1;
			}
		}

		usleep(100);
	}
	return 0;
}


static int write_2_to_file(char *recv_buff,int len)
{
	FILE *fp = fopen("/opt/ack","wb+");
	if(!fp)
	{
		printf("open ack file error\n");
		return -1;
	}
	fwrite(recv_buff,len,1,fp);
	fclose(fp);

	return 0;
}


/**
 * 功能:策略发送到客户端
 *
 */
static int plc_sync_send_data(t_data_buff *file_info,t_dir_param *dir_param)
{
	int i ;
	int sockfd = -1;

	char *ptemp = NULL;
	char *paddr = NULL;
	unsigned int    ipv4;
	unsigned char   iptype;
	unsigned short  port;
	char    ipv6[16] = {0};

	int ret     = 0;
	int rcv_len = 0;
	char recv_buff[512]     = {0};

	int plc_sync_num = 0;
	t_sync_ipport *plc_sync_ipport = NULL;

	plc_sync_num = dir_param->sync_ipport_count;
	plc_sync_ipport = dir_param->sync_ipport;

	for(i = 0 ; i<plc_sync_num; i++)
	{
		if(plc_sync_ipport[i].sockfd != -1)
		{
			ret = plc_send_data(plc_sync_ipport[i].sockfd,file_info->buff,file_info->offset);
			if(ret == -1)
			{
				//fprintf(stdout,"send plc to(%s:%d) failure!\n",plc_sync_ipport[i].ipstr,plc_sync_ipport[i].port);
				plc_sync_ipport[i].sockfd = -1;
				continue;
			}
			if((rcv_len = recv(plc_sync_ipport[i].sockfd,recv_buff,100,0)) < 0)
			{
				close(plc_sync_ipport[i].sockfd);
				plc_sync_ipport[i].sockfd = -1;
				continue;
			}
			//fprintf(stdout,"send plc to(%s:%d)\n",plc_sync_ipport[i].ipstr,plc_sync_ipport[i].port);
			//if(rcv_len)
			//{
				//printf("recv buff:%s\n",recv_buff);
			//}
		}
		else
		{
			//printf("ipstr:%s,port:%u\n",plc_sync_ipport[i].ipstr,plc_sync_ipport[i].port);
			ptemp = strstr(plc_sync_ipport[i].ipstr,":");
			if(ptemp)       ///< ipv6
			{
				bzero(ipv6, 16);
				iptype  = 6;
				port    = htons(plc_sync_ipport[i].port);
				inet_pton(AF_INET6,plc_sync_ipport[i].ipstr,&ipv6);
				paddr = ipv6;
				//paddr = plc_sync_ipport[i].ipstr;
			}
			else
			{
				iptype  = 4;
				port    = htons(plc_sync_ipport[i].port);
				inet_pton(AF_INET,plc_sync_ipport[i].ipstr,&ipv4);
				paddr   = (char *)&ipv4;
			}

			sockfd =  eu_create_tcp_conn(paddr,port,iptype);
			if(sockfd == -1)
			{
				continue;
			}

			ret = plc_send_data(sockfd,file_info->buff,file_info->offset);
			if(ret == -1)
			{
				//fprintf(stdout,"send plc to(%s:%d) failure!\n",plc_sync_ipport[i].ipstr,plc_sync_ipport[i].port);
				close(sockfd);
				plc_sync_ipport[i].sockfd = -1;
				continue;
			}
			int reset = 0;
			while(1)
			{
				rcv_len = recv(sockfd,recv_buff,512,0);
				if(rcv_len <= 0)
				{
					sleep(1);
					reset++;
					if(reset >= 300)
					{
						close(sockfd);
						sockfd = -1;
						printf("can't get ack buff\n");
						break;
					}
				}
				else
				{
					break;
				}
			}



			/*
			if((rcv_len = recv(sockfd,recv_buff,512,0)) < 0)
			{
				close(sockfd);
				sockfd = -1;
				continue;
			}*/

			if(rcv_len > 0)
			{
				//fprintf(stdout,"send plc to(%s:%d),sockfd:%d\n",plc_sync_ipport[i].ipstr,plc_sync_ipport[i].port,sockfd);
				//printf("get recv buff\n");
				write_2_to_file(recv_buff,rcv_len);
			}
			plc_sync_ipport[i].sockfd = sockfd;
		}
	}
	return 0;
}


/**
 * 功能:按行读入文件,按照提供的回调函数进行处理
 * \param [in] filename    需要处理的文件名
 * \param [in] result      文件被处理之后的结果(用户参数)
 * \param [in] line_func   读取文件每一行之后执行的回调
 *
 */
static int file_read_cont_by_line(const char *filename,void *result,FILE_LINE_CALLBACK line_func)
{
	FILE *fp = NULL;
	fp = fopen(filename,"r");
	if(!fp)
	{
		fprintf(stderr,"fopen file(%s) failure.(%s)!\n",filename,strerror(errno));
		return 0;
	}
	int len = 0;
	int ret = 0;
	char buff[2048] = {0};
	t_data_buff *file_info = NULL;
	file_info = (t_data_buff *)result;
	file_info->line_num = 1;
	while(fgets(buff,2048,fp) != NULL)
	{
		str_all_trim(buff);
		if(!memcmp(buff,"\r\n",2) || buff[0] == '\n' || buff[0] == '\r' || buff[0] == '#')
		{
			continue;
		}
		len = strlen(buff);
		buff[len - 1] = '\0';
		ret += line_func(result,buff,len);
		file_info->line_num++;
	}
	fclose(fp);

	return ret;
}

/**
 * 功能:递归遍历目录,根据提供的回调函数处理遍历的文件名
 * \param [in] dirname 目录名称
 * \param [in] userparam 用户参数
 * \param [in] file_func 递归遍历对文件的处理函数
 *
 */
static int dir_traver_by_recursion(char *dirname,void *userparam,DIR_FILE_CALLBACK file_func)
{
	DIR *l_dir;
	l_dir = opendir(dirname);
	if(l_dir == NULL)
	{
		fprintf(stderr,"[%s] open dir %s failure!\n",__FUNCTION__,dirname);
		return -1;
	}

	char l_path[128]  = {0};
	struct dirent *l_filename;
	struct stat s;

	while((l_filename = readdir(l_dir)) != NULL )
	{
		if(l_filename->d_name[0] == '.')
		{
			continue;
		}

		snprintf(l_path,128,"%s/%s",dirname,l_filename->d_name);
		lstat(l_path,&s);                   ///< 获取文件的相关信息
		if(S_ISDIR(s.st_mode))
		{
			dir_traver_by_recursion(l_path,userparam,file_func);
		}
		else
		{
			#if 0
			printf("the file:%s\n",l_path);
			#else
			if(strstr(l_path,"txt") || strstr(l_path,"TXT"))
			{
				file_func(userparam,l_path);
			}
			#endif
		}
	}

	closedir(l_dir);
	return 0;
}


/**
 * 功能: 单字节数据的输出
 * \param [in] data_buff  数据输出缓冲区
 * \param [in] indata 需要处理的数据
 * \param [in] len    indata的长度
 *
 */
inline int print_rt_1b(t_data_buff *data_buff,void *indata, int len)
{
	char *pbuff     = NULL;
	char *endptr    = NULL;
	unsigned char val   = strtol(indata,&endptr,10);

	pbuff = data_buff->buff  + data_buff->offset;
	*((unsigned char *)pbuff) = val ;
	data_buff->offset += len;       ///< 1 字节
	return len;
}

inline int print_rt_2b(t_data_buff *data_buff,void *indata, int len)
{
	char *pbuff         = NULL;
	char *endptr        = NULL;
	unsigned short  val = 0;
	val = strtol(indata,&endptr,10);

	pbuff = data_buff->buff + data_buff->offset;
	*((unsigned short *)pbuff) = htons(val);
	data_buff->offset += len;       ///< 2字节
	return len;
}

inline int print_rt_4b(t_data_buff *data_buff,void *indata, int len)
{
	char *pbuff     = NULL;
	char *endptr    = NULL;
	unsigned int val = 0;
	val = strtol(indata,&endptr,10);

	pbuff = data_buff->buff + data_buff->offset;
	*((unsigned int *)pbuff) = htonl(val);
	data_buff->offset += len;       ///< 4字节
	return len;
}

inline int print_rt_8b(t_data_buff *data_buff,void *indata, int len)
{
	char *pbuff         = NULL;
	char *endptr        = NULL;
	unsigned long  val  = 0;
	val = strtol(indata,&endptr,10);

	pbuff = data_buff->buff + data_buff->offset;
	val   = htonl64(val);
	*((unsigned long *)pbuff) = val;
	data_buff->offset += 8;

	return len;
}


inline int print_rt_16b(t_data_buff *data_buff,void *indata, int len)
{
	return 0;
}


/**
 * 将ipv4地址转化为网络字节序
 *
 */
inline int print_rt_ipv4(t_data_buff *data_buff,void *indata, int len)
{
	char *pbuff = NULL;
	int ret     = 0;
	pbuff   = data_buff->buff + data_buff->offset;
	ret     = inet_pton(AF_INET,indata,pbuff);
	if(ret <= 0 && len > 0)
	{
		fprintf(stderr,"[%s] convert ipstr(%s) failure!\n",__FUNCTION__,(char *)indata);
		data_buff->offset += 4;
		return -1;
	}
	data_buff->offset += 4;
	return len;
}

inline int print_rt_ipv6(t_data_buff *data_buff,void *indata, int len)
{
	char *pbuff = NULL;
	int ret     = 0;
	pbuff   = data_buff->buff + data_buff->offset;
	ret     = inet_pton(AF_INET6,indata,pbuff);
	if(ret <= 0)
	{
		fprintf(stderr,"[%s] convert ipstr(%s) failure!\n",__FUNCTION__,(char *)indata);
		data_buff->offset += 16;
		return -1;
	}
	data_buff->offset += 16;
	return len;

}

inline int print_rt_strb(t_data_buff *data_buff,void *indata, int len)
{
	char *pbuff = NULL;
	pbuff   = data_buff->buff + data_buff->offset;
	if(len > 0)
	{
		memcpy(pbuff,indata,len);
	}
	data_buff->offset += len;
	return len;
}

inline int print_rt_timestampb(t_data_buff *data_buff, void *indata, int len)
{
	unsigned int stamp;
	char *pbuff = data_buff->buff + data_buff->offset;

	stamp = (unsigned int)str_timestamp_to_unix((char *)indata);
	//printf("stamp is %u\n", stamp);

	*((unsigned int *)pbuff) = htonl(stamp);
	data_buff->offset += len;
	return len;
}

inline int print_rt_binary(t_data_buff *data_buff,void *indata, int len)
{
	char *pbuff			= NULL;
	char *binary_str	= NULL;
	int  binary_len		= 0;

	binary_str = (char *)indata;
	pbuff   = data_buff->buff + data_buff->offset;

	if(strlen(binary_str) % 8)
	{
		binary_len = strlen(binary_str) / 8 + 1;
	}
	else
	{
		binary_len = strlen(binary_str) / 8;
	}

	if((len > 0) && (binary_len <= len))	// 若二进制字符串长度 / 8，转换成实际的字符串长度
	{
		int i = 0, j = 0;
		char *data = (char *)malloc(len+1);
		bzero(data, len+1);

		while(i < binary_len)
		{
			#if 0
			if('1' == binary_str[i*8])
			{
				data[i] = 0x01;
			}
			#endif
			data[i] = binary_str[i*8] - '0';
			
			for(j=1; j<8; j++)
			{
				data[i] = data[i] << 1;
				if('1' == binary_str[i*8+j])
				{
					data[i] += 0x01;
				}
				else if('0' == binary_str[i*8+j])
				{
					data[i] += 0x0;
				}
				else if('\0' == binary_str[i*8+j])
				{
					data[i] += 0x0;
				}
				else
				{
					continue;
				}
			}
			i++;
		}
		memcpy(pbuff,data,len);
	}
	data_buff->offset += len;
	return len;
}

static bool hex_to_decimal(char *hex, char *decimal, int len)
{
	int hex_len = 0;
	int i;
	
	if(strlen(hex) % 2)
	{
		hex_len = strlen(hex) / 2 + 1;
	}
	else
	{
		hex_len = strlen(hex) / 2;
	}
	if(!hex_len)
	{
		return true;
	}
	if(hex_len > len)
	{
		return false;
	}

	char *data = (char *)malloc(len+1);
	bzero(data, len+1);
	for(i=0; i<hex_len; i++)
	{
		if(hex[i*2])
		{
			if(hex[i*2] <= '9' && hex[i*2] >= '0')
			{
				data[i] = hex[i*2] - '0';
			}
			else if(hex[i*2] <= 'f' && hex[i*2] >= 'a')
			{
				data[i] = hex[i*2] - 'a' + 10;
			}
			else if(hex[i*2] <= 'F' && hex[i*2] >= 'A')
			{
				data[i] = hex[i*2] - 'A' + 10;
			}

			if(hex[i*2+1])
			{
				if(hex[i*2+1] <= '9' && hex[i*2+1] >= '0')
				{
					data[i] = hex[i*2+1] - '0' + data[i] * 16;
				}
				else if(hex[i*2+1] <= 'f' && hex[i*2+1] >= 'a')
				{
					data[i] = hex[i*2+1] - 'a' + 10 + data[i] * 16;
				}
				else if(hex[i*2+1] <= 'F' && hex[i*2+1] >= 'A')
				{
					data[i] = hex[i*2+1] - 'A' + 10 + data[i] * 16;
				}
			}
			else
			{
				data[i] = data[i] * 16;
			}
		}
		else
		{
			continue;
		}
	}

	memcpy(decimal, data, len);

	return true;
}

inline int print_rt_hex(t_data_buff *data_buff,void *indata, int len)
{
	char *pbuff			= NULL;
	char *hex_str		= NULL;
	char decimal[256]	= {0};

	hex_str		= (char *)indata;
	pbuff		= data_buff->buff + data_buff->offset;

	if(len > 256)
	{
		printf("hex number is too long!");
		return len;
	}
	if(!hex_to_decimal(hex_str, decimal, len))
	{
		return 0;
	}

	memcpy(pbuff, decimal, len);
	data_buff->offset += len;
	return len;
}

typedef struct st_filed_helper
{
	char type[64];
	FILED_OUTPUT_CALLBACK  output_func;
}t_filed_helper;


t_filed_helper filed_desc_arr[] =
{
	{"char",     print_rt_1b},
	{"uchar",    print_rt_1b},
	{"short",    print_rt_2b},
	{"ushort",   print_rt_2b},
	{"int",      print_rt_4b},
	{"uint",     print_rt_4b},
	{"long",     print_rt_8b},
	{"ulong",    print_rt_8b},
	{"timestamp",print_rt_timestampb},
	{"string",   print_rt_strb},
	{"ipv4",     print_rt_ipv4},
	{"ipv6",     print_rt_ipv6},
};


/**
 * 功能:处理读取的文件的每一行
 * @param [in] result 行信息处理之后的结果信息
 * @param [in] linebuf 每一行的内容
 * @param [in] linelen 每一行的内容的长度
 *
 */
int plc_proc_file_line(void *result,char *linebuf,int linelen)
{
	t_data_buff *p_file;

	long valen = 0;
	char str_key[1024]    = {0};
	char str_type[1024]   = {0};
	char str_len[1024]    = {0};
	char str_val[1024]    = {0};
	char *endptr        = NULL;

	p_file = (t_data_buff *)result;
	//sscanf(linebuf,"%s",str_key);
	if (!sscanf(linebuf,"{key=%[^,],type=%[^,],len=%[^,],val=%[^~]}",str_key,str_type,str_len,str_val))

	{
		fprintf(stderr,"num:%d,line format maybe error.(%s)!\n",p_file->line_num,linebuf);
		exit(0);
	}
	/*
	if('}' != str_val[strlen(str_val) - 1] && strcmp(str_key,"msglen"))
	{
		return 0;
	}
	*/

	if((atoi(str_len)+ 1) > strlen(str_val))
	{
		if('}' == str_val[strlen(str_val) - 1])
		{
			str_val[strlen(str_val) - 1] = '\0';
		}
	}
	else if((atoi(str_len) + 1) == strlen(str_val))
	{
		str_val[atoi(str_len)] = '\0';
	}
	else
	{
		str_val[strlen(str_val) - 1] = '\0';
	}

	#if 0
	if(strlen(str_val) > 2)
	{
		if('}' == str_val[strlen(str_val) - 1])
		{
			str_val[strlen(str_val) - 1] = '\0';
		}
		else if('\r' == str_val[strlen(str_val) - 1])
		{
			if('}' == str_val[strlen(str_val) - 2])
			{
			str_val[strlen(str_val) - 2] = '\0';}
		}
	}
	else if(strlen(str_val) == 1 && *str_val == '}')
	{
		*str_val = '\0';
	}
	#endif

    str_all_trim(str_key);
    str_all_trim(str_type);
    str_all_trim(str_len);
    str_all_trim(str_val);

	if(!strncmp("timestamp", str_key, strlen("timestamp")))
	{
		time_t t;
		t = time(NULL);
		struct tm *lt;
		int ii = time(&t);
		lt = localtime(&t);
		bzero(str_val, 1024);
		strftime(str_val, 1024, "%Y-%m-%d %H:%M:%S",lt);
		//printf("time is %s\n", str_val);
	}
	
    valen = strtol(str_len,&endptr,10);
    if(!strcmp(str_type,"string") && (valen > 0) && (valen < strlen(str_val)))
    {
        fprintf(stderr,"[%ld] valen less than strlen(%s)\n",valen,str_val);
        exit(0);
    }
	#if 0
    if(!strcmp(str_key,"msglen"))       ///< 如果是msglen
    {
        p_file->pdatalen = p_file->buff + p_file->offset;
        p_file->offset  += valen;
        return valen;
    }
	#endif

    int i = 0;
    int arr_size;
    arr_size = sizeof(filed_desc_arr)/sizeof(filed_desc_arr[0]);

    for(i = 0; i<arr_size; i++)
    {
        if(!strcmp(str_type,filed_desc_arr[i].type))
        {
            filed_desc_arr[i].output_func(p_file,str_val,valen);
        }
    }

    return valen;
}

/**
 * 功能: 处理目录下的文件
 * \param [in] userparam 传入的参数
 * \param [in] filename 目录下文件名
 *
 */
int plc_proc_dir_file(void *userparam,char *filename)
{
    t_data_buff *file_info;
    t_dir_param *dir_param;

    dir_param = (t_dir_param *)userparam;
    file_info = &(dir_param->file_info);

    file_info->offset   = 0;
    file_info->pdatalen = NULL;
    file_info->line_num = 0;
    file_read_cont_by_line(filename,file_info,plc_proc_file_line);

    //*(unsigned int *)(file_info->pdatalen) = htonl(file_info->offset);   ///< 计算字节长度

    plc_sync_send_data(file_info,dir_param);            ///< 发送

    int file_len;
    char outfilename[128];

    file_len = strlen(filename);
    if(!memcmp(filename + file_len - 3,"txt",3)
        || !memcmp(filename + file_len -3,"TXT",3) )
    {
        strncpy(outfilename,filename,file_len);
        outfilename[file_len - 1] = '\0';
        outfilename[file_len - 2] = 'c';
        outfilename[file_len - 3] = 'u';
        FILE *fp = fopen(outfilename,"w");
        if(!fp)
        {
            fprintf(stderr,"[%s] fopen outfilename failure.(%s)!\n",__FUNCTION__,outfilename);
            return 0;
        }

        fwrite(file_info->buff,file_info->offset,1,fp);

        fclose(fp);
        //fprintf(stdout,"convert %s\t->\t%s\n",filename,outfilename);
    }

    return 0;
}

/**
 * 功能:plc_parse_ipport 解析策略下发的ip和端口
 * \param [in] ipport_str  ip端口字符串
 * \param [in] sync_ipport 同步ip和端口字符串结构体
 *
 */
#if 0
static int plc_parse_ipport(const char *ipport_str, t_sync_ipport *sync_ipport)
{
    char       temp_str[50]={0};
	int        i,j,k=0;
	//printf("parse target %s \n",target_str);
	j=0;
	for(i=0;i<strlen(ipport_str);i++)
	{
		if('('==ipport_str[i])
		{
			k=0;
			i++;
			while(':'!=ipport_str[i]&&i<strlen(ipport_str))
			{
				sync_ipport[j].ipstr[k++] = ipport_str[i++];
			}
			i++;
			sync_ipport[j].ipstr[k++] = 0;
			k=0;
			while(')'!=ipport_str[i]&&i<strlen(ipport_str))
			{
				temp_str[k++] = ipport_str[i++];
			}
			temp_str[k]=0;
			sync_ipport[j].port = atoi(temp_str);
			j++;
		}
	}
	return j;
}
#endif
#if 1
static int plc_parse_ipport(const char *ipport_str, t_sync_ipport *sync_ipport)
{
    char        temp_str[50]={0};
    char        *port;
    int         i,j,k,ipstr_len=0;
    j=0;
    
    for(i=0;i<strlen(ipport_str);i++){
        bzero(temp_str, 50);
        if('('==ipport_str[i]){         
            k=0;
            i++;
            while(')'!= ipport_str[i]){
                temp_str[k] = ipport_str[i];
                i++;
                k++;
            }
            port = strrchr(temp_str, ':');
            ipstr_len = port - temp_str;
            strncpy(sync_ipport[j].ipstr, temp_str, ipstr_len);
            sync_ipport[j].port = atoi(port + 1);
            j++;
        }
    }
    return j;

}
#endif


#if 0
static int test_plc()
{
    char infilename[128]  = "traffic_flow.txt";
    char outfilename[128] = "traffic_flow.uc";

    t_data_buff  file_info;
    file_info.offset   = 0;
    file_info.buff_len = 1024 * 1024 * 5;
    file_info.buff = calloc(1,file_info.buff_len);
    if(!file_info.buff)
    {
        fprintf(stderr,"[%s] calloc mem for file_info.buff failure!\n",__FUNCTION__);
        exit(1);
    }

    file_read_cont_by_line(infilename,&file_info,plc_proc_file_line);

    *(unsigned int *)(file_info.pdatalen) = htonl(file_info.offset);   ///< 计算字节长度

    ///< 发送数据到客户端

    FILE *fp = fopen(outfilename,"w");
    if(!fp)
    {
        fprintf(stderr,"[%s] fopen outfilename failure.(%s)!\n",__FUNCTION__,outfilename);
        return 0;
    }

    fwrite(file_info.buff,file_info.offset,1,fp);
    fclose(fp);
    return 0;
}

#endif


int main(int argc, char **argv)
{
    if(argc == 2)
    {
        if(strcmp(argv[1],"-v") == 0)
        {
            printf("the version:%s\n",VERSION);
            exit(0);
        }
    }
    if(argc < 3)
    {
        printf("usage %s [policy path] [(ip:port) like \"(10.8.0.250:50000)\"]\n",argv[0]);
        exit(0);
    }
    int plc_len = 0;
    char plc_dir[128]   = {0};
    strncpy(plc_dir,argv[1],128);       ///< 保存目录
    plc_len = strlen(plc_dir);
    if(plc_dir[plc_len - 1] == '/')
    {
        plc_dir[plc_len -1] = '\0';
    }

    int i = 0;
    int plc_sync_num = 0 ;
    t_dir_param  dir_param;
    dir_param.sync_ipport_capacity = 100;
    dir_param.sync_ipport = calloc(dir_param.sync_ipport_capacity,sizeof(t_sync_ipport));
    if(!dir_param.sync_ipport)
    {
        fprintf(stderr,"[%s] calloc mem for sync_ipport!\n",__FUNCTION__);
        exit(0);
    }
    for(i=0; i< dir_param.sync_ipport_capacity; i++)
    {
        dir_param.sync_ipport[i].sockfd = -1;
    }

    plc_sync_num = plc_parse_ipport(argv[2],dir_param.sync_ipport);
    dir_param.sync_ipport_count = plc_sync_num;

    dir_param.file_info.pdatalen = NULL;
    dir_param.file_info.offset   = 0;
    dir_param.file_info.buff_len = 1024 * 1024 * 5;
    dir_param.file_info.buff     = calloc(1,dir_param.file_info.buff_len);
    if(!dir_param.file_info.buff)
    {
        fprintf(stderr,"[%s] calloc mem for file_info.buff failure!\n",__FUNCTION__);
        exit(0);
    }

    ///< 遍历目录
    dir_traver_by_recursion(plc_dir,(void *)&dir_param,plc_proc_dir_file);

    ///< 释放内存和套接字
    for(i = 0 ; i<plc_sync_num; i++)
    {
        if(dir_param.sync_ipport[i].sockfd != -1)
        {
            fprintf(stderr,"close sockfd:%d\n",dir_param.sync_ipport[i].sockfd);
            close(dir_param.sync_ipport[i].sockfd);
        }
        dir_param.sync_ipport[i].sockfd = -1;
    }
    free(dir_param.file_info.buff);
    free(dir_param.sync_ipport);

    return 0;

}



