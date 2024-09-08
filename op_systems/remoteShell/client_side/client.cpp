#include<stdlib.h>
#include<stdio.h>
#include<unistd.h>
#include<sys/socket.h>
#include<sys/wait.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<string.h>
#include<pthread.h>
#include<fcntl.h>


void init_client_side(struct sockaddr_in* address,char* ip,int port,int domain,int type) {

    bzero(address,sizeof(struct sockaddr_in));
    address->sin_family = domain;
    address->sin_port = htons(port);
    if(inet_pton(domain,ip,&address->sin_addr) == -1) {
        fprintf(stderr,"Erro while loading address\n");
        exit(EXIT_FAILURE);
    }

}

void start_run(struct sockaddr_in* address,int type,const char* filename) {

    int sock_fd = socket(address->sin_family,type,0);
    if(sock_fd < 0) {
        perror("error occured while creating socket\n");
        exit(EXIT_FAILURE);
    }
    if(connect(sock_fd,(struct sockaddr*)address,sizeof(*address)) == -1) {
        perror("Could not connect to the server\n");
        exit(EXIT_FAILURE);
    }
    int fd = -1;
    if(filename) {
        fd = open(filename,O_CREAT | O_WRONLY,0666);
    }
    bool running = true;
    char BUFFER[BUFSIZ];
    memset(BUFFER,'\0',BUFSIZ);
    while (running) {
        fgets(BUFFER,BUFSIZ-1,stdin);
        if(!strcmp(BUFFER,"exit")) {
            running = false;
            break;
        }
        BUFFER[strlen(BUFFER)-1] = '\0';
        printf("read: %s\n",BUFFER);
        write(sock_fd,BUFFER,strlen(BUFFER));
        ssize_t read_bytes = read(sock_fd,BUFFER,BUFSIZ-1);
        BUFFER[read_bytes] = '\0';
        if(!filename) {
            printf("%s\n",BUFFER);
        } else {
                
            write(fd,BUFFER,strlen(BUFFER));
        }
        printf("Done writing\n");
    }
    close(fd);

}


int main(int argc, char** argv) {

    if(argc != 4) {
        perror("Wrong usage\n");
        exit(EXIT_FAILURE);
    }

    int port = atoi(argv[2]);
    char* ip = argv[1];
    struct sockaddr_in address;
    init_client_side(&address,ip,port,AF_INET,SOCK_STREAM);
    if(!strcmp(argv[3],"-stdin")) {
        start_run(&address,SOCK_STREAM,nullptr);
    } else {
        start_run(&address,SOCK_STREAM,"output.txt");
    }


    exit(EXIT_SUCCESS);
}