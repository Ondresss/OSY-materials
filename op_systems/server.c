#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<sys/socket.h>
#include <string.h>
#include<netdb.h>


#define BUFFER_LEN 1024

int main(int argc,char ** argv) {

    struct sockaddr_in server_address;
    struct sockaddr_in client_address;
    socklen_t client_len = sizeof(client_address);
    int port_number = atoi(argv[1]);
    int server_sock_fd,client_sock_fd = -1;
    char input_buffer[BUFFER_LEN];
    memset(input_buffer,0,BUFFER_LEN);

    memset(&server_address,0,sizeof(server_address));
    memset(&client_address,0,sizeof(client_address));
    server_address.sin_port = htons(port_number);
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

    printf("You are using port: %d\n",port_number);
    server_sock_fd = socket(AF_INET,SOCK_STREAM,0);

    if(server_sock_fd < 0) {
        fprintf(stderr,"Could not create socket\n");
    }


    int bind_result = bind(server_sock_fd,(struct sockaddr * )&server_address,sizeof(server_address));
    if(bind_result == -1) {
        fprintf(stderr,"Could not bind\n");
    }

    (void)listen(server_sock_fd,5);
    printf("Server is listening\n");
    client_sock_fd = accept(server_sock_fd,(struct sockaddr*)&client_address,&client_len);
    if(client_sock_fd == -1) {
        fprintf(stderr,"could not accept\n");
        
    }
    printf("New client accepted\n");
    while (1)
    {
         int result = read(client_sock_fd,input_buffer,BUFFER_LEN-1);
         if(result == -1) {
            printf("Could not read message\n");
            continue;
         } else {
            input_buffer[result] = '\0';
            printf("Recieved from the client: \t%s\n",input_buffer);
         }
      
         if(!strcmp(input_buffer,"exit") || !result) {
            close(client_sock_fd);
            printf("waiting for another client\n");
            client_sock_fd = accept(server_sock_fd,(struct sockaddr*)&client_address,&client_len);
            if(client_sock_fd == -1) {
                fprintf(stderr,"could not accept\n");
                
            } else {
                printf("New client accepted\n");
            }

            
         }
    }
    


    return EXIT_SUCCESS;
}