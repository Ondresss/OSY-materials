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
#include<fcntl.h>

#include<sys/stat.h>

void prepare_address(struct sockaddr_in* address,char* ip_address,int port,int domain){

    bzero(address,sizeof(*address));
    address->sin_family = domain;
    address->sin_port = port;
    if(inet_pton(domain,ip_address,&address->sin_addr) != 1){

        fprintf(stderr,"Could not setup address\n");
        exit(EXIT_FAILURE);
    }
    
}



void send_file(int client_fd,char* filePath) {
    if(access(filePath,F_OK) == -1) {
        fprintf(stderr,"File doesnt exist\n");
        exit(EXIT_FAILURE);
    }

    char buffer[BUFSIZ];
    int fd = open(filePath,O_RDONLY);
    if(fd < 0) {
        fprintf(stderr,"Could not open a file\n");
        exit(EXIT_FAILURE);
    }

    int bytes_read = read(fd,buffer,BUFSIZ -1);  
    while(bytes_read > 0) {
        buffer[bytes_read] = '\0';
        printf("Line to sent: %s\n",buffer);
        if(write(client_fd,buffer,strlen(buffer)) == -1){
            printf("Could not send line\n");
            exit(EXIT_FAILURE);
        }

        bytes_read = read(fd,buffer,BUFSIZ -1);
    }
    write(client_fd,"DONE",4);
}


int main(int argc,char** argv) {

 
    char output_buffer[BUFSIZ];
    struct sockaddr_in server_address;
    int port = atoi(argv[2]);
    char* address = argv[1];
    int client_fd = -1;
    int valid = 0;
    prepare_address(&server_address,address,port,AF_INET);
    client_fd = socket(AF_INET,SOCK_STREAM,0);
    if(client_fd == -1) {
        fprintf(stderr,"%s\n",strerror(errno));
        exit(EXIT_FAILURE);
    }
    if(connect(client_fd,(struct sockaddr*)&server_address,sizeof(server_address)) == -1) {
        fprintf(stderr,"%s\n",strerror(errno));
        exit(EXIT_FAILURE);
    }
    struct stat file_stat;
    (void)stat(argv[3],&file_stat);
    long file_size = file_stat.st_size;
    sprintf(output_buffer,"%lu",file_size);

    while (1)
    {


        if(!valid) {
            write(client_fd,output_buffer,strlen(output_buffer));
            printf("Wrote to server:%s\n",output_buffer);
            read(client_fd,output_buffer,BUFSIZ-1);
            printf("Recieved:%s\n",output_buffer);
            if(!strcmp(output_buffer,"OK")){
                
                valid = 1;
            } else {
                printf("Invalid file\n");
                close(client_fd);
                exit(EXIT_FAILURE);
            }
            printf("Recieved back from the server:%s\n",output_buffer);
            
        } else {
            send_file(client_fd,argv[3]);

            printf("File sent\n");
            close(client_fd);
            exit(EXIT_SUCCESS);
        } 
    }
    


    return EXIT_SUCCESS;
}


