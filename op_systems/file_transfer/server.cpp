#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<netdb.h>
#include<netinet/in.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<string.h>
#include<errno.h>
#include<ctype.h>
#include<semaphore.h>
#include<fcntl.h>


sem_t semaphore;


void prepare_address(struct sockaddr_in* address,char* ip_address,int port,int domain){

    bzero(address,sizeof(*address));
    address->sin_family = domain;
    address->sin_port = port;
    if(inet_pton(domain,ip_address,&address->sin_addr) != 1){

        fprintf(stderr,"Could not setup address\n");
        exit(EXIT_FAILURE);
    }
    
}

int check_len(char* input) {
    for(int i = 0; i < strlen(input);i++) {
        if(isdigit(input[i] == 0)) {
            return 0;
        }
    }
    return 1;
}


int main(int argc,char** argv) {

    char input_buffer[BUFSIZ];
    int sem_state = 1;
    int init = 0;
    int save_file_fd = -1;
    struct sockaddr_in server_address;
    struct sockaddr_in client_address;
    socklen_t client_len = sizeof(client_address);
    int port = atoi(argv[2]);
    int server_sock_fd,client_sock_fd = -1;
    char* address = argv[1];
    prepare_address(&server_address,address,port,AF_INET);
    server_sock_fd = socket(AF_INET,SOCK_STREAM,0);
    if(server_sock_fd == -1) {
        fprintf(stderr,"%s\n",strerror(errno));
        exit(EXIT_FAILURE);
    }

    if(bind(server_sock_fd,(struct sockaddr*)&server_address,sizeof(server_address)) == -1) {
        fprintf(stderr,"%s\n",strerror(errno));
        exit(EXIT_FAILURE);
    }

    printf("Server is listening\n");
    (void)listen(server_sock_fd,SOMAXCONN);


    while (1)
    {
        printf("Server is waiting for client\n");
        client_sock_fd = accept(server_sock_fd,(struct sockaddr*)&client_address,&client_len);
        printf("New client accepted\n");
        save_file_fd = open("output.txt",O_CREAT | O_APPEND | O_WRONLY);
        int bytes_read = read(client_sock_fd,input_buffer,BUFSIZ-1);
        while(bytes_read > 0) {
            input_buffer[bytes_read] = '\0';
            printf("Recieved from the client%s\n",input_buffer);
            if(!init) {
                if(check_len(input_buffer) == 0) {
                    printf("Recieved invalid file\n");
                    const char* invalid = "Invalid file length\n";
                    write(client_sock_fd,invalid,strlen(invalid));
                    close(client_sock_fd);
                    continue;
                }

                write(client_sock_fd,"OK",2);
                printf("OK SENT\n");
                init++; 
            } else {
                write(save_file_fd,input_buffer,strlen(input_buffer));
                printf("Line written on the server\n");
            }
            bytes_read = read(client_sock_fd,input_buffer,BUFSIZ-1);
        }

        printf("Client disconnected\n");
        close(client_sock_fd);
        close(save_file_fd);



    }
    


    







    return EXIT_SUCCESS;
}