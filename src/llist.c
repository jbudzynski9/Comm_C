//#include "list.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>

typedef struct users{
  char *name;
  int socket;
  struct users *next;
} us;

void printList(us *head, char **list){

    printf("Users on-line:\n");
    while (head != NULL){
        printf("%s: %d\n", head->name, head->socket);
        head = head->next;
    }
}

void add(us **head, int socket, char *pname){

  us *new = (us *) malloc (sizeof(us));
  us *temp = *head;
  new->name = (char *) malloc (sizeof(char));
  bzero(new->name, sizeof(new->name));
  strcpy(new->name, pname);
  new->socket = socket;
  new->next = NULL;

  while (temp->next != NULL){
    temp = temp->next;
  }

  temp->next = new;
  printf("Logging user: %s, socket: %d\n",new->name, socket);
  return;

}

void delete(us **head, char *pname){
    us *point = *head;
    us *temp = NULL;

   if (point != NULL && (strcmp(point->name, pname) == 0)){
        printf("Logging out user: %s, socket: %d\n", point->name, point->socket);
        *head = point->next;

        free(point->name);
        free(point);
        return;
   }

    while (point != NULL && (strcmp(point->name, pname) != 0)){
        temp = point;
        point = point->next;
    }

    if (point == NULL){
        printf("There is no user: %s on the server\n", pname);
        return;
    }

    temp->next = point->next;

    printf("Logging out user: %s, socket: %d\n", point->name, point->socket);

    free(point->name);
    free(point);
}

int findUser(us *head, char *pname){

    head = head->next;
    while (head != NULL){
      if (strcmp(head->name, pname) == 0){
        return head->socket;
        break;
      }

      head = head->next;
    }

    return 0;
}

void findSocket(us *head, int sock, char **sender){


    *sender=(char*)malloc(sizeof(char));
    head = head->next;
    while (head != NULL){
      if (head->socket==sock){
        *sender=head->name;
        break;
      }

      head = head->next;
    }
}

int usersList(us *head, char **list){

    head = head->next;
    *list=(char*)malloc(sizeof(char));
    strcpy(*list,"7");
    while (head != NULL){
      strcat(*list,".");
      strcat(*list,head->name);
      head = head->next;
    }
    strcat(*list,"\n");

    return 0;
}
int broadcast(us *head, char *msg){

    head = head->next;
    while (head != NULL){
      write(head->socket, msg, strlen(msg));
      head = head->next;
    }
    return 0;
}
