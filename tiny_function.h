#ifndef _TINY_FUNCTION_H
#define _TINY_FUNCTION_H

#include <arpa/inet.h> /* inet_ntoa */
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/sendfile.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#define LISTENQ 1024 /* second argument to listen() */
#define MAXLINE 1024 /* max length of a line */
#define RIO_BUFSIZE 8192

#ifndef DEFAULT_PORT
#define DEFAULT_PORT 9999 /* use this port if none given as arg to main() */
#endif

#ifndef FORK_COUNT
#define FORK_COUNT 4
#endif

#ifndef NO_LOG_ACCESS
#define LOG_ACCESS
#endif

typedef struct {
    int rio_fd;                /* descriptor for this buf */
    int rio_cnt;               /* unread byte in this buf */
    char *rio_bufptr;          /* next unread byte in this buf */
    char rio_buf[RIO_BUFSIZE]; /* internal buffer */
} rio_t;

/* Simplifies calls to bind(), connect(), and accept() */
typedef struct sockaddr SA;

typedef struct {
    char filename[512];
    off_t offset; /* for support Range */
    size_t end;
} http_request;

typedef struct {
    const char *extension;
    const char *mime_type;
} mime_map;

// https://developer.mozilla.org/en-US/docs/Web/HTTP/Basics_of_HTTP/MIME_types/Common_types

void rio_readinitb(rio_t *rp, int fd);
ssize_t writen(int fd, void *usrbuf, size_t n);
ssize_t rio_readlineb(rio_t *rp, void *usrbuf, size_t maxlen);
void format_size(char *buf, struct stat *stat);
void handle_directory_request(int out_fd, int dir_fd, char *filename);
int open_listenfd(int port);
void url_decode(char *src, char *dest, int max);
void parse_request(int fd, http_request *req);
void log_access(int status, struct sockaddr_in *c_addr, http_request *req);
void client_error(int fd, int status, char *msg, char *longmsg);
void serve_static(int out_fd, int in_fd, http_request *req, size_t total_size);
void process(int fd, struct sockaddr_in *clientaddr);
void print_help();

#endif /* _TINY_FUNCTION_H */