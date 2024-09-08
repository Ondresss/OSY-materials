#include<sys/socket.h>
#include<sys/wait.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<string.h>
#include<stdio.h>
#include<errno.h>
#include<stdlib.h>

#define MAX_CONS 10

int init_server(struct sockaddr_in * address,int domain,int port,char* ip_address,int type);
bool start_server(struct sockaddr_in * address,int server_fd);