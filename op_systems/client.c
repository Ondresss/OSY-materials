#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<arpa/inet.h>
#include<sys/socket.h>
#include <string.h>
#include<netdb.h>
#include<errno.h>



int main(int argc,char ** argv) {

    int port_number = atoi(argv[2]);
    char* address = argv[1];
    int sock_fd = -1;

    char buffer[1024];
    memset(buffer,0,1024);

    printf("Address for connection is: %s\t port number is: %d\n",address,port_number);

    struct sockaddr_in server_address;
    memset(&server_address,0,sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port_number);
    if (inet_pton(AF_INET, address, &server_address.sin_addr) <= 0) { 
        fprintf(stderr, "Invalid address/ Address not supported \n");
        return EXIT_FAILURE;
    }
    
    sock_fd = socket(AF_INET,SOCK_STREAM,0);
    if(sock_fd < 0) {
        fprintf(stderr,"Could not create socket\n");
        exit(EXIT_FAILURE);
    }

    int res_connection = connect(sock_fd,(struct sockaddr*)&server_address,sizeof(server_address));
    if(res_connection == -1) {
        printf("%s\n",strerror(errno));
        fprintf(stderr,"Could not connect to the server\n");
        exit(EXIT_FAILURE);
    }

    printf("Write a message to the server: ");
    read(0,buffer,1023);
    (void)write(sock_fd,buffer,strlen(buffer));
    close(sock_fd);
    return EXIT_SUCCESS;
}