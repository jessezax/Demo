#include "libx.h"
#include <curl/curl.h>

static ssize_t xftp_upload_callback(void *ptr, size_t size, size_t nmemb, void *stream)
{
    return fread(ptr, size, nmemb, stream);
}

bool xftp_upload(const char * file_path,const char * upload_file_as,const char * rename_file_to,
                const char * url,const char * user_name,const char * passwd)
{
    CURL *curl;
    CURLcode res;
    FILE *fp;
    off_t fsize;
    bool  retval = false;

    struct curl_slist *headerlist = NULL;
    char upload_file_as_cmd [256];
    char rename_file_to_cmd [256];
    char login[256];

    fsize = file_size(file_path);
    if (fsize < 0) {
        return false;
    }

    /* 获得FILE类型变量 */ 
    fp = fopen(file_path, "rb");
    if (!fp) {
        return false;
    }

    /* 获得curl操作符 */ 
    curl = curl_easy_init();
    if(curl) {
        /*建立一个传递给libcurl的命令列表 */ 
        if (upload_file_as) {
            snprintf(upload_file_as_cmd,256,"RNFR %s",upload_file_as);
            headerlist = curl_slist_append(headerlist, upload_file_as_cmd);
            if (rename_file_to) {
                snprintf(rename_file_to_cmd,256,"RNTO %s",rename_file_to);
                headerlist = curl_slist_append(headerlist, rename_file_to_cmd);
            }
        }
        if (user_name) {
            snprintf(login, 256, "%s:%s", user_name,passwd?passwd:"");
            curl_easy_setopt(curl, CURLOPT_USERPWD, login);
        }

        /* 使用curl提供的Read功能 */ 
        curl_easy_setopt(curl, CURLOPT_READFUNCTION, xftp_upload_callback);

        /* 上传使能 */ 
        curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);

        /* 设置特定目标 */ 
        curl_easy_setopt(curl, CURLOPT_URL, url);

        /* 传递最后一个FTP命令以在传输后运行 */ 
        curl_easy_setopt(curl, CURLOPT_POSTQUOTE, headerlist);

        /*指定上传文件 */ 
        curl_easy_setopt(curl, CURLOPT_READDATA, fp);

        /*设置要上传的文件的大小（可选） */ 
        curl_easy_setopt(curl, CURLOPT_INFILESIZE_LARGE,(curl_off_t)fsize);

        /* 运行 */ 
        res = curl_easy_perform(curl);
        /* 容错处理 */ 
        if(CURLE_OK == res) {
            retval = true;
        }
        /* 清除FTP命令列表 */ 
        curl_slist_free_all(headerlist);

        /*释放所有curl资源 */ 
        curl_easy_cleanup(curl);
    }
    fclose(fp); /*关闭本地文件 */ 
    return retval;
}

#if 0
static ssize_t xftp_download_callback(void *buffer, size_t size, size_t nmemb, void *stream)
{
    return fwrite(buffer, size, nmemb, stream);
}
#endif

bool xftp_download(const char * url,const u16 port,const char * user_name,const char * passwd,f_xftp_download_callback cb,void * cb_args)
{
    CURL *curl;
    CURLcode res;
    bool retval = false;
    char login[256];
    char buf[1024] = {0};
    char *ftp_url = buf;

    if(strstr(url, ":") == strrchr(url, ':')){
        //strncpy(ftp_url, url, strlen(url));
        
        char *location = strstr(url+6, "/");
        int len = location - url;
        snprintf(ftp_url, 1024, "%.*s:%u%s", len, url, port, location);
    }else{
        snprintf(ftp_url, 1024, "%.*s[%.*s]:%u%s",
                                6, url,
                                39, url + 6,
                                port,
                                url + 45);      
    }
    curl = curl_easy_init();
    if(curl) {
        if (user_name) {
            snprintf(login, 256, "%s:%s", user_name,passwd?passwd:"");
            curl_easy_setopt(curl, CURLOPT_USERPWD, login);
        }

        curl_easy_setopt(curl, CURLOPT_URL,ftp_url);//下载指定的文件
        
        
        /* 定义回调函数，以便在需要写入数据时进行调用 */ 
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, cb);
        /*设置一个指向我们的结构的指针传递给回调函数*/ 
        curl_easy_setopt(curl, CURLOPT_WRITEDATA,cb_args);

        //curl_easy_setopt(curl, CURLOPT_SSH_AUTH_TYPES, CURLSSH_AUTH_AGENT);

        /* 打开完整的协议/调试输出*/ 
        curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

        res = curl_easy_perform(curl);
        if(CURLE_OK == res) {
            retval = true;
        }
        /* 释放所有curl资源*/ 
        curl_easy_cleanup(curl);
    }
    return retval;
}

__constructor__(101)
static void curl_init(void)
{
    curl_global_init(CURL_GLOBAL_ALL);
}
