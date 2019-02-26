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
#include "list.h"

#define PORT 3113
#define MAX_CONN 16
#define MAX_EVENTS 32
#define BUFF_SIZE 1000

static void epollCtl(int ep_fd, int fd, uint32_t events)
{
    struct epoll_event event;
    event.events = events;
    event.data.fd = fd;
    if (epoll_ctl(ep_fd, EPOLL_CTL_ADD, fd, &event) == -1)
    {
        perror("epollCtl fail()\n");
        exit(1);
    }
}

static void setSocket(struct sockaddr_in *addr)
{
    //bzero((char *)addr, sizeof(struct sockaddr_in));
    addr->sin_family = AF_INET;
    addr->sin_addr.s_addr = INADDR_ANY;
    addr->sin_port = htons(PORT);
}

static int nonBlock(int sockfd){
  if (fcntl(sockfd, F_SETFL, fcntl(sockfd, F_GETFD, 0) | O_NONBLOCK) ==-1){
      return -1;
  }
  return 0;
}



static int decode(char msg[BUFF_SIZE],char **name, char **msg1){

  if(msg[1]=='.'){
    char msgcp[BUFF_SIZE];
    strcpy(msgcp,msg);

    int a=atoi(strtok(msgcp,"."));
    *name=strtok(NULL,".");
    *msg1=strtok(NULL,".");

    return a;
  }
  else{
    return 0;
  }
}

void main()
{
  int i, n, s, event_count,code,op,error_flag,f;
  int ep_fd, sock_fd, cl_sock_fd;
  char *user="";
  char *msg1="";
  char msg2[BUFF_SIZE];
  char msg[BUFF_SIZE];
  char *nickname="";
  char *sender="";
  char *list="7.";
  struct sockaddr_in server_addr, client_addr;
  struct epoll_event events[MAX_EVENTS],ev;

  us *head = malloc(sizeof(us));
  head->name = NULL;
  head->socket = 0;
  head->next = NULL;

  sock_fd = socket(AF_INET, SOCK_STREAM, 0);

  setSocket(&server_addr);
  bind(sock_fd, (struct sockaddr *)&server_addr, sizeof(server_addr));

  nonBlock(sock_fd);

  listen(sock_fd, MAX_CONN);

  ep_fd = epoll_create(1);
  epollCtl(ep_fd, sock_fd, EPOLLIN | EPOLLOUT | EPOLLET);

  s = sizeof(client_addr);
  bzero(msg, sizeof(msg));

  while(1){
    event_count = epoll_wait(ep_fd, events, MAX_EVENTS, -1);
    for (i = 0; i < event_count; i++){
      if (events[i].data.fd == sock_fd){
          cl_sock_fd = accept(sock_fd, (struct sockaddr *)&client_addr, &s);
          printf("Connected with client (socket [%i])\n", cl_sock_fd);

          nonBlock(cl_sock_fd);
          epollCtl(ep_fd, cl_sock_fd, EPOLLIN | EPOLLET | EPOLLRDHUP | EPOLLHUP);

        }
        else if (events[i].events & EPOLLIN){
            bzero(msg, sizeof(msg));
            n = read(events[i].data.fd, msg, sizeof(msg));
            if(n>0)
              op=decode(msg,&nickname,&msg1);

            switch(op){
              case 2:
                f=findUser(head,nickname);
                if(f==0){
                  error_flag=0;
                  add(&head,events[i].data.fd,nickname);
                }
                else error_flag=2;
                bzero(msg, sizeof(msg));
                break;
              case 3:
                //printf("name: %s\n", nickname);
                //printf("msg: %s\n", msg1);
                f=findUser(head,nickname);
                if(f!=0){
                  error_flag=0;
                  findSocket(head,events[i].data.fd,&sender);
                  sprintf(msg2,"4.%s.%s",sender,msg1);
                  write(f, msg2, strlen(msg2));
                  bzero(msg2, sizeof(msg2));
                }
                else error_flag=3;
                bzero(msg, sizeof(msg));
                break;
              case 5:
                error_flag=0;
                findSocket(head,events[i].data.fd,&sender);
                sprintf(msg2,"5.%s.%s",sender,msg1);
                broadcast(head,msg2);
                bzero(msg2, sizeof(msg2));

                break;
              case 6:
                usersList(head,&list);
                write(events[i].data.fd, list, strlen(list));
                bzero(list, sizeof(list));
                break;
              default:
                error_flag=1;
                break;
            }
            bzero(msg, sizeof(msg));

            if (n <= 0){
              break;
            }

            else{
              switch(error_flag){
                case 0:
                  bzero(msg, sizeof(msg));
                  sprintf(msg,"ACK: 1.%i\n",error_flag);
                  write(events[i].data.fd, msg, strlen(msg));
                  bzero(msg, sizeof(msg));
                  break;
                case 1:
                  bzero(msg, sizeof(msg));
                  sprintf(msg,"ACK: 1.%i.Invalid code\n",error_flag);
                  write(events[i].data.fd, msg, strlen(msg));
                  bzero(msg, sizeof(msg));
                  break;
                case 2:
                  bzero(msg, sizeof(msg));
                  sprintf(msg,"ACK: 1.%i.Nickname already exists\n",error_flag);
                  write(events[i].data.fd, msg, strlen(msg));
                  bzero(msg, sizeof(msg));
                  break;
                case 3:
                  bzero(msg, sizeof(msg));
                  sprintf(msg,"ACK: 1.%i.There is no such user\n",error_flag);
                  write(events[i].data.fd, msg, strlen(msg));
                  bzero(msg, sizeof(msg));
                  break;
              }
            }

        }
        else{
          printf("unexpected\n");
        }
        if (events[i].events & (EPOLLRDHUP | EPOLLHUP)){
          printf("Client disconnected\n");
          epoll_ctl(ep_fd, EPOLL_CTL_DEL, events[i].data.fd, NULL);
          close(events[i].data.fd);
          continue;
        }
    }
  }
}
