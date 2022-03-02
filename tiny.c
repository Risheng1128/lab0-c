#include "tiny_function.h"

int main(int argc, char **argv)
{
    if (argc > 1 && (!strcmp(argv[1], "-h") || !strcmp(argv[1], "--help"))) {
        print_help();
        return 0;
    }

    struct sockaddr_in clientaddr;
    int default_port = DEFAULT_PORT, listenfd, connfd;
    char buf[256];
    char *path = getcwd(buf, 256);
    socklen_t clientlen = sizeof(clientaddr);
    if (argc == 2) {
        if (argv[1][0] >= '0' && argv[1][0] <= '9') {
            default_port = atoi(argv[1]);
        } else {
            path = argv[1];
            if (chdir(path) != 0) {
                perror(path);
                exit(1);
            }
        }
    } else if (argc == 3) {
        default_port = atoi(argv[2]);
        path = argv[1];
        if (chdir(path) != 0) {
            perror(path);
            exit(1);
        }
    }
    printf("serve directory '%s'\n", path);

    listenfd = open_listenfd(default_port);
    if (listenfd > 0) {
        printf("listen on port %d, fd is %d\n", default_port, listenfd);
    } else {
        perror("ERROR");
        exit(listenfd);
    }
    // Ignore SIGPIPE signal, so if browser cancels the request, it
    // won't kill the whole process.
    signal(SIGPIPE, SIG_IGN);

    int i = 0;
    for (; i < FORK_COUNT; i++) {
        int pid = fork();
        if (pid == 0) {  //  child
            while (1) {
                connfd = accept(listenfd, (SA *) &clientaddr, &clientlen);
                process(connfd, &clientaddr);
                close(connfd);
            }
        } else if (pid > 0) {  //  parent
            printf("child pid is %d\n", pid);
        } else {
            perror("fork");
        }
    }

    while (1) {
        connfd = accept(listenfd, (SA *) &clientaddr, &clientlen);
        process(connfd, &clientaddr);
        close(connfd);
    }

    return 0;
}
