#ifndef __essentials_h
#define __essentials_h

#include<sys/types.h>
#include<sys/socket.h>
#include<sys/time.h>
#include<netinet/in.h>
#include<time.h>
#include<arpa/inet.h>
#include<errno.h>
#include<fcntl.h>
#include<netdb.h>
#include<signal.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/stat.h>
#include<sys/uio.h>
#include<unistd.h>
#include<sys/wait.h>
#include<sys/un.h>
#include<poll.h>
#include<sys/select.h>
#include<pthread.h>
#include<semaphore.h>
#include<stdarg.h>
#include<syslog.h>
#include<sys/epoll.h>
#include<sys/ioctl.h>

#define bzero(ptr,n) memset(ptr,0,n)
#define SA struct sockaddr
#define ull unsigned long long

#define min(a,b) ((a) < (b) ? (a) : (b))
#define max(a,b) ((a) > (b) ? (a) : (b))

#define MAXLINE 4096
#define BUFFSIZE 8192
#define LISTENQ 10
#define SERV_PORT 9877
#define SERV_PORT_STR "9877"

#endif