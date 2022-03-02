#include "tiny_function.h"

mime_map meme_types[] = {
    {".aac", "audio/aac"},
    {".abw", "application/x-abiword"},
    {".arc", "application/x-freearc"},
    {".avi", "video/x-msvideo"},
    {".azw", "application/vnd.amazon.ebook"},
    {".bin", "application/octet-stream"},
    {".bmp", "image/bmp"},
    {".bz", "application/x-bzip"},
    {".bz2", "application/x-bzip2"},
    {".csh", "application/x-csh"},
    {".css", "text/css"},
    {".csv", "text/csv"},
    {".doc", "application/msword"},
    {".docx",
     "application/vnd.openxmlformats-officedocument.wordprocessingml.document"},
    {".eot", "application/vnd.ms-fontobject"},
    {".epub", "application/epub+zip"},
    {".gz", "application/gzip"},
    {".gif", "image/gif"},
    {".htm", "text/html"},
    {".html", "text/html"},
    {".ico", "image/vnd.microsoft.icon"},
    {".ics", "text/calendar"},
    {".jar", "application/java-archive"},
    {".jpeg", "image/jpeg"},
    {".jpg", "image/jpeg"},
    {".js", "text/javascript"},
    {".json", "application/json"},
    {".jsonld", "application/ld+json"},
    {".mid", "audio/midi audio/x-midi"},
    {".midi", "audio/midi audio/x-midi"},
    {".mjs", "text/javascript"},
    {".mp3", "audio/mpeg"},
    {".mp4", "video/mp4"},
    {".mpeg", "video/mpeg"},
    {".mpkg", "application/vnd.apple.installer+xml"},
    {".odp", "application/vnd.oasis.opendocument.presentation"},
    {".ods", "application/vnd.oasis.opendocument.spreadsheet"},
    {".odt", "application/vnd.oasis.opendocument.text"},
    {".oga", "audio/ogg"},
    {".ogv", "video/ogg"},
    {".ogx", "application/ogg"},
    {".opus", "audio/opus"},
    {".otf", "font/otf"},
    {".png", "image/png"},
    {".pdf", "application/pdf"},
    {".php", "application/x-httpd-php"},
    {".ppt", "application/vnd.ms-powerpoint"},
    {".pptx",
     "application/"
     "vnd.openxmlformats-officedocument.presentationml.presentation"},
    {".rar", "application/vnd.rar"},
    {".rtf", "application/rtf"},
    {".sh", "application/x-sh"},
    {".svg", "image/svg+xml"},
    {".swf", "application/x-shockwave-flash"},
    {".tar", "application/x-tar"},
    {".tif", "image/tiff"},
    {".tiff", "image/tiff"},
    {".ts", "video/mp2t"},
    {".ttf", "font/ttf"},
    {".txt", "text/plain"},
    {".vsd", "application/vnd.visio"},
    {".wav", "audio/wav"},
    {".weba", "audio/webm"},
    {".webm", "video/webm"},
    {".webp", "image/webp"},
    {".woff", "font/woff"},
    {".woff2", "font/woff2"},
    {".xhtml", "application/xhtml+xml"},
    {".xls", "application/vnd.ms-excel"},
    {".xlsx",
     "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet"},
    {".xml", "text/xml"},
    {".xul", "application/vnd.mozilla.xul+xml"},
    {".zip", "application/zip"},
    {".3gp", "video/3gpp"},
    {".3g2", "video/3gpp2"},
    {".7z", "application/x-7z-compressed"},
    {NULL, NULL},
};

void rio_readinitb(rio_t *rp, int fd)
{
    rp->rio_fd = fd;
    rp->rio_cnt = 0;
    rp->rio_bufptr = rp->rio_buf;
}

ssize_t writen(int fd, void *usrbuf, size_t n)
{
    size_t nleft = n;
    char *bufp = usrbuf;

    while (nleft > 0) {
        ssize_t nwritten;
        if ((nwritten = write(fd, bufp, nleft)) <= 0) {
            if (errno == EINTR) { /* interrupted by sig handler return */
                nwritten = 0;     /* and call write() again */
            } else {
                return -1; /* errorno set by write() */
            }
        }
        nleft -= nwritten;
        bufp += nwritten;
    }
    return n;
}


/*
 * rio_read - This is a wrapper for the Unix read() function that
 *    transfers min(n, rio_cnt) bytes from an internal buffer to a user
 *    buffer, where n is the number of bytes requested by the user and
 *    rio_cnt is the number of unread bytes in the internal buffer. On
 *    entry, rio_read() refills the internal buffer via a call to
 *    read() if the internal buffer is empty.
 */
/* $begin rio_read */
static ssize_t rio_read(rio_t *rp, char *usrbuf, size_t n)
{
    int cnt;
    while (rp->rio_cnt <= 0) { /* refill if buf is empty */

        rp->rio_cnt = read(rp->rio_fd, rp->rio_buf, sizeof(rp->rio_buf));
        if (rp->rio_cnt < 0) {
            if (errno != EINTR) { /* interrupted by sig handler return */
                return -1;
            }
        } else if (rp->rio_cnt == 0) { /* EOF */
            return 0;
        } else
            rp->rio_bufptr = rp->rio_buf; /* reset buffer ptr */
    }

    /* Copy min(n, rp->rio_cnt) bytes from internal buf to user buf */
    cnt = n;
    if (rp->rio_cnt < n) {
        cnt = rp->rio_cnt;
    }
    memcpy(usrbuf, rp->rio_bufptr, cnt);
    rp->rio_bufptr += cnt;
    rp->rio_cnt -= cnt;
    return cnt;
}

/*
 * rio_readlineb - robustly read a text line (buffered)
 */
ssize_t rio_readlineb(rio_t *rp, void *usrbuf, size_t maxlen)
{
    int n;
    char c, *bufp = usrbuf;

    for (n = 1; n < maxlen; n++) {
        int rc;
        if ((rc = rio_read(rp, &c, 1)) == 1) {
            *bufp++ = c;
            if (c == '\n') {
                break;
            }
        } else if (rc == 0) {
            if (n == 1) {
                return 0; /* EOF, no data read */
            } else {
                break; /* EOF, some data was read */
            }
        } else {
            return -1; /* error */
        }
    }
    *bufp = 0;
    return n;
}

void format_size(char *buf, struct stat *stat)
{
    if (S_ISDIR(stat->st_mode)) {
        snprintf(buf, MAXLINE, "%s", "[DIR]");
    } else {
        off_t size = stat->st_size;
        if (size < 1024) {
            snprintf(buf, size, "%lu", size);
        } else if (size < 1024 * 1024) {
            snprintf(buf, size, "%.1fK", (double) size / 1024);
        } else if (size < 1024 * 1024 * 1024) {
            snprintf(buf, size, "%.1fM", (double) size / 1024 / 1024);
        } else {
            snprintf(buf, size, "%.1fG", (double) size / 1024 / 1024 / 1024);
        }
    }
}

void handle_directory_request(int out_fd, int dir_fd, char *filename)
{
    char buf[MAXLINE], m_time[32], size[16];
    struct stat statbuf;
    snprintf(buf, MAXLINE, "HTTP/1.1 200 OK\r\n%s%s%s%s%s",
             "Content-Type: text/html\r\n\r\n", "<html><head><style>",
             "body{font-family: monospace; font-size: 13px;}",
             "td {padding: 1.5px 6px;}", "</style></head><body><table>\n");
    writen(out_fd, buf, strlen(buf));
    DIR *d = fdopendir(dir_fd);
    struct dirent *dp;
    int ffd;
    while ((dp = readdir(d)) != NULL) {
        if (!strcmp(dp->d_name, ".") || !strcmp(dp->d_name, "..")) {
            continue;
        }
        if ((ffd = openat(dir_fd, dp->d_name, O_RDONLY)) == -1) {
            perror(dp->d_name);
            continue;
        }
        fstat(ffd, &statbuf);
        strftime(m_time, sizeof(m_time), "%Y-%m-%d %H:%M",
                 localtime(&statbuf.st_mtime));
        format_size(size, &statbuf);
        if (S_ISREG(statbuf.st_mode) || S_ISDIR(statbuf.st_mode)) {
            char *str = S_ISDIR(statbuf.st_mode) ? "/" : "";
            snprintf(buf, MAXLINE,
                     "<tr><td><a "
                     "href=\"%s%s\">%s%s</a></td><td>%s</td><td>%s</td></tr>\n",
                     dp->d_name, str, dp->d_name, str, m_time, size);
            writen(out_fd, buf, strlen(buf));
        }
        close(ffd);
    }
    snprintf(buf, MAXLINE, "</table></body></html>");
    writen(out_fd, buf, strlen(buf));
    closedir(d);
}

char *default_mime_type = "text/plain";
static const char *get_mime_type(char *filename)
{
    char *dot = strrchr(filename, '.');
    if (dot) {  // strrchar Locate last occurrence of character in string
        mime_map *map = meme_types;
        while (map->extension) {
            if (strcmp(map->extension, dot) == 0) {
                return map->mime_type;
            }
            map++;
        }
    }
    return default_mime_type;
}


int open_listenfd(int port)
{
    int listenfd, optval = 1;
    struct sockaddr_in serveraddr;

    /* Create a socket descriptor */
    if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        return -1;
    }

    /* Eliminates "Address already in use" error from bind. */
    if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, (const void *) &optval,
                   sizeof(int)) < 0) {
        return -1;
    }

    // 6 is TCP's protocol number
    // enable this, much faster : 4000 req/s -> 17000 req/s
    if (setsockopt(listenfd, 6, TCP_CORK, (const void *) &optval, sizeof(int)) <
        0) {
        return -1;
    }

    /* Listenfd will be an endpoint for all requests to port
       on any IP address for this host */
    memset(&serveraddr, 0, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serveraddr.sin_port = htons((unsigned short) port);
    if (bind(listenfd, (SA *) &serveraddr, sizeof(serveraddr)) < 0) {
        return -1;
    }

    /* Make it a listening socket ready to accept connection requests */
    if (listen(listenfd, LISTENQ) < 0) {
        return -1;
    }
    return listenfd;
}

void url_decode(char *src, char *dest, int max)
{
    char *p = src;
    char code[3] = {0};
    while (*p && --max) {
        if (*p == '%') {
            memcpy(code, ++p, 2);
            *dest++ = (char) strtoul(code, NULL, 16);
            p += 2;
        } else {
            *dest++ = *p++;
        }
    }
    *dest = '\0';
}

void parse_request(int fd, http_request *req)
{
    rio_t rio;
    char buf[MAXLINE], method[MAXLINE], uri[MAXLINE];
    req->offset = 0;
    req->end = 0; /* default */

    rio_readinitb(&rio, fd);
    rio_readlineb(&rio, buf, MAXLINE);
    sscanf(buf, "%1023s %1023s", method, uri); /* version is not cared */
    /* read all */
    while (buf[0] != '\n' && buf[1] != '\n') { /* \n || \r\n */
        rio_readlineb(&rio, buf, MAXLINE);
        if (buf[0] == 'R' && buf[1] == 'a' && buf[2] == 'n') {
            sscanf(buf, "Range: bytes=%zu-%zu", &req->offset, &req->end);
            // Range: [start, end]
            if (req->end != 0) {
                req->end++;
            }
        }
    }
    char *filename = uri;
    if (uri[0] == '/') {
        filename = uri + 1;
        int length = strlen(filename);
        if (length == 0) {
            filename = ".";
        } else {
            int i = 0;
            for (; i < length; ++i) {
                if (filename[i] == '?') {
                    filename[i] = '\0';
                    break;
                }
            }
        }
    }
    url_decode(filename, req->filename, MAXLINE);
}

#ifdef LOG_ACCESS
void log_access(int status, struct sockaddr_in *c_addr, http_request *req)
{
    printf("%s:%d %d - '%s' (%s)\n", inet_ntoa(c_addr->sin_addr),
           ntohs(c_addr->sin_port), status, req->filename,
           get_mime_type(req->filename));
}
#endif

void client_error(int fd, int status, char *msg, char *longmsg)
{
    char buf[MAXLINE];
    snprintf(buf, MAXLINE, "HTTP/1.1 %d %s\r\n", status, msg);
    snprintf(buf + strlen(buf), MAXLINE, "Content-length: %zu\r\n\r\n",
             strlen(longmsg));
    snprintf(buf + strlen(buf), MAXLINE, "%s", longmsg);
    writen(fd, buf, strlen(buf));
}

void serve_static(int out_fd, int in_fd, http_request *req, size_t total_size)
{
    char buf[256];
    if (req->offset > 0) {
        snprintf(buf, 256, "HTTP/1.1 206 Partial\r\n");
        snprintf(buf + strlen(buf), 256, "Content-Range: bytes %zu-%zu/%zu\r\n",
                 req->offset, req->end, total_size);
    } else {
        snprintf(buf, 256, "HTTP/1.1 200 OK\r\nAccept-Ranges: bytes\r\n");
    }
    snprintf(buf + strlen(buf), 256, "Cache-Control: no-cache\r\n");
    // snprintf(buf + strlen(buf), 256, "Cache-Control: public,
    // max-age=315360000\r\nExpires: Thu, 31 Dec 2037 23:55:55 GMT\r\n");

    snprintf(buf + strlen(buf), 256, "Content-length: %lu\r\n",
             req->end - req->offset);
    snprintf(buf + strlen(buf), 256, "Content-type: %s\r\n\r\n",
             get_mime_type(req->filename));

    writen(out_fd, buf, strlen(buf));
    off_t offset = req->offset; /* copy */
    while (offset < req->end) {
        if (sendfile(out_fd, in_fd, &offset, req->end - req->offset) <= 0) {
            break;
        }
#ifdef LOG_ACCESS
        printf("offset: %u \n\n", (unsigned int) offset);
#endif
        close(out_fd);
        break;
    }
}

void process(int fd, struct sockaddr_in *clientaddr)
{
#ifdef LOG_ACCESS
    printf("accept request, fd is %d, pid is %d\n", fd, getpid());
#endif
    http_request req;
    parse_request(fd, &req);

    struct stat sbuf;
    int status = 200, ffd = open(req.filename, O_RDONLY, 0);
    if (ffd <= 0) {
        status = 404;
        char *msg = "File not found";
        client_error(fd, status, "Not found", msg);
    } else {
        fstat(ffd, &sbuf);
        if (S_ISREG(sbuf.st_mode)) {
            if (req.end == 0) {
                req.end = sbuf.st_size;
            }
            if (req.offset > 0) {
                status = 206;
            }
            serve_static(fd, ffd, &req, sbuf.st_size);
        } else if (S_ISDIR(sbuf.st_mode)) {
            status = 200;
            handle_directory_request(fd, ffd, req.filename);
        } else {
            status = 400;
            char *msg = "Unknow Error";
            client_error(fd, status, "Error", msg);
        }
        close(ffd);
    }
#ifdef LOG_ACCESS
    log_access(status, clientaddr, &req);
#endif
}

void print_help()
{
    printf("TINY WEBSERVER HELP\n");
    printf("tiny            #use default port, serve current dir\n");
    printf("tiny /tmp       #use default port, serve given dir\n");
    printf("tiny 1234       #use given port, serve current dir\n");
    printf("tiny /tmp 1234  #use given port, serve given dir\n");
    printf("default port is %d.\n", DEFAULT_PORT);
}