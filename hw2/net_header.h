#ifndef __net_header_h
#define __net_header_h

#include<netinet/in.h>
#include<signal.h>
#include<stdio.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<stdlib.h>
#include<string.h>
#include<fcntl.h>
#include<unistd.h>
#define LISTENQ 1024
#define PORT 8080
#define SA struct sockaddr
typedef struct users{
	char username[128];
	int fd;
	struct users *next;
}users; 
users *head;
#endif
