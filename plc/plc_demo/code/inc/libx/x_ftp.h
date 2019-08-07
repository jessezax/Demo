#ifndef __X_FTP_H__
#define __X_FTP_H__

bool xftp_upload(const char * file_path,const char * upload_file_as,
                const char * rename_file_to,const char * url,
                const char * user_name,const char * passwd);

typedef ssize_t (*f_xftp_download_callback)(void *buffer, size_t size, size_t nmemb, void *cb_args);

bool xftp_download(const char * url, const u16 port, const char * user_name,const char * passwd,f_xftp_download_callback cb,void * cb_args);

#endif
