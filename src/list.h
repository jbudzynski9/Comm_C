#ifndef LIST_H
#define LIST_H

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

typedef struct users
{
  char *name;
  int socket;
  struct users *next;
} us;

void printList(us *head, char **list);

void add(us **head, int socket, char *pname);

void delete(us **head, char *pname);

int findUser(us *head, char *pname);

void findSocket(us *head, int sock, char **sender);

int usersList(us *head, char **list);

int broadcast(us *head, char *msg);

#endif
