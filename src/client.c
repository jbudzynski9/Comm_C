#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

#define PORT 3113
#define BUFF_SIZE 1000

void main(){
    int n, msg_len, sock_fd;
    char msg[BUFF_SIZE];
    struct sockaddr_in addr;

    sock_fd = socket(AF_INET, SOCK_STREAM, 0);

    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(PORT);

    if (connect(sock_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0){
        perror("connect()");
        exit(1);
    }

    for (;;){
        bzero(msg, sizeof(msg));
        printf("Insert message: ");
        fgets(msg, sizeof(msg), stdin);
        msg_len = strlen(msg) - 1;
        msg[msg_len] = '\0';
        write(sock_fd, msg, msg_len + 1);

        bzero(msg, sizeof(msg));
        while (errno != EAGAIN && (n = read(sock_fd, msg, sizeof(msg))) > 0){
            printf("%s\n", msg);
            bzero(msg, sizeof(msg));

            msg_len -= n;
            if (msg_len <= 0)
                break;
        }
    }
    close(sock_fd);
}
