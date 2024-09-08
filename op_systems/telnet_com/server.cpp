#include<unistd.h>
#include<pthread.h>
#include<stdlib.h>
#include<netdb.h>
#include<arpa/inet.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<string.h>
#include<stdio.h>
#include<netinet/in.h>
#include <errno.h>


#define MAX_CON 3


struct Sock_desc {

    int current_num;
    int fds[MAX_CON];
    int current_active;
};



void init_address(struct sockaddr_in * address_str,int type,char* address,int port) {

    address_str->sin_port = htons(port);
    address_str->sin_family = AF_INET;
    if(inet_pton(AF_INET,address,&address_str->sin_addr) == -1){
        fprintf(stderr,"Please enter valid address\n");
    }
    
}

void broad_cast(Sock_desc* descs,int excluded,char* message) {
    for(int i = 0; i < descs->current_num;i++){
        if(descs->fds[i] != excluded) {
            (void)write(descs->fds[i],message,strlen(message));
        }
    }
    return;
}

void* handle_client(void* arguments) {
    Sock_desc* sock_desc = (Sock_desc*)arguments;
    int active = sock_desc->current_active;
    char buffer[BUFSIZ];
    const char* response_back = "AHOJ\n";
    while (1)
    {
        int bytes_read = read(active,buffer,BUFSIZ-1);
        if(bytes_read > 0) {
            buffer[bytes_read] = '\0';
            printf("Response from the client:%s\n",buffer);
            broad_cast(sock_desc,active,buffer);
        } else {
            pthread_exit(arguments);
        }
        
        
    }
    

    pthread_exit(arguments);
}


int main(int argc,char** argv) {

    struct sockaddr_in server_address;
    struct sockaddr_in client_address;
    Sock_desc sock_desc;
    socklen_t client_len = sizeof(client_address);
    int port = atoi(argv[2]);
    int server_fd,client_fd = -1;
    init_address(&server_address,AF_INET,argv[1],port);
    server_fd = socket(AF_INET,SOCK_STREAM,0);
    sock_desc.current_num = 0;
    if(server_fd == -1) {
        fprintf(stderr,"%s\n",strerror(errno));
        exit(EXIT_FAILURE);
    }

    if(bind(server_fd,(struct sockaddr*)&server_address,sizeof(server_address)) == -1){
        fprintf(stderr,"%s\n",strerror(errno));
        exit(EXIT_FAILURE);
    }

    if(listen(server_fd,SOMAXCONN) == -1){
        fprintf(stderr,"%s\n",strerror(errno));
        exit(EXIT_FAILURE);
    }
    pthread_t thr[MAX_CON];
    while (1)
    {
        if(sock_desc.current_num > 3) {
            printf("Cannot accept more clients\n");
            continue;
        }
        client_fd = accept(server_fd,(struct sockaddr*)&client_address,&client_len);
        if(client_fd == -1) {
            fprintf(stderr,"%s\n",strerror(errno));
            exit(EXIT_FAILURE);
        }
        sock_desc.current_num++;
        sock_desc.fds[sock_desc.current_num-1] = client_fd;
        sock_desc.current_active = client_fd;
        printf("Client is accepted\n");
        if(pthread_create(&thr[sock_desc.current_num-1],NULL,handle_client,&sock_desc) != 0) {
            fprintf(stderr,"%s\n",strerror(errno));
        }
      


    }

    for(int i = 0; i < sock_desc.current_num;i++) {
        if(pthread_join(thr[i],NULL) != 0) {
            fprintf(stderr,"Could not wait\n");
            exit(EXIT_FAILURE);
        }
    }
    
    


    return EXIT_SUCCESS;
}