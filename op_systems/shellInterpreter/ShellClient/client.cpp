#include<sys/socket.h>
#include<sys/wait.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<string.h>
#include<stdio.h>
#include<errno.h>
#include<stdlib.h>
#include<errno.h>
#include<fcntl.h>

int init_client(struct sockaddr_in * address,int port,int domain,char* ip_address) {
    int client_fd = -1;
    bzero(address,sizeof(*address));
    address->sin_port = htons(port);
    address->sin_family = domain;
    if(inet_pton(domain,ip_address,&address->sin_addr) == -1) {
        fprintf(stderr,"Invalid address: %s\n",strerror(errno));
        exit(EXIT_FAILURE);
    }

    client_fd = socket(domain,SOCK_STREAM,0);
    if(client_fd < 0) {
        fprintf(stderr,"%s\n",strerror(errno));
        exit(EXIT_FAILURE);
    }

    return client_fd;
}


void handle_response(int client_fd,const char* flag) {

    char buffer[BUFSIZ] = {0};
    if(!strcmp(flag,"-f")) {
        ssize_t bytes_read = read(client_fd,buffer,BUFSIZ-1);
        int fd = open("response.txt",O_CREAT | O_WRONLY,0766);
        if(fd < 0) {
            fprintf(stderr,"Could not open a file\n");
            exit(EXIT_FAILURE);
        }

        while (bytes_read > 0) {
            buffer[bytes_read-1] = '\0';
            write(fd,buffer,strlen(buffer));

            bytes_read = read(client_fd,buffer,BUFSIZ-1);
        
        }

        if(bytes_read == -1) {
            fprintf(stderr,"Could not read back response\n");
            exit(EXIT_FAILURE);
        }

        close(fd);
        
    }



}



int main(int argc,char** argv) {

    if(argc != 4) {
        fprintf(stderr,"Invalid use\n");
        exit(EXIT_FAILURE);
    }
    char buffer[BUFSIZ] = {0};
    struct sockaddr_in address;
    int client_fd = -1;
    char* ip_address = argv[1];
    int port = atoi(argv[2]);
    bool client_running = false;
    client_fd = init_client(&address,port,AF_INET,ip_address);

    printf("Waiting for connection\n");
    if(connect(client_fd,(struct sockaddr*)&address,sizeof(address)) == -1) {
        fprintf(stderr,"Could not connect\n");
        exit(EXIT_FAILURE);
    }
    printf("Connected\n");

    client_running = true;

    while (client_running) {
        long bytes_read = read(STDIN_FILENO,buffer,BUFSIZ-1);
        if(bytes_read <= 0) {
            fprintf(stderr,"Could not read from stdin\n");
            exit(EXIT_FAILURE);
        }
        buffer[bytes_read-1] = '\0';
        printf("%s\n",buffer);
        if(write(client_fd,buffer,strlen(buffer)) == -1) {
            fprintf(stderr,"Could not write into the socket\n");
            exit(EXIT_FAILURE);
        }

        handle_response(client_fd,argv[3]);

        
    }

    close(client_fd);
    


    





    exit(EXIT_SUCCESS);
}