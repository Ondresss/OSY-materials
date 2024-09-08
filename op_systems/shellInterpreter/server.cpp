#include<stdlib.h>
#include<string.h>
#include<stdio.h>

#include<unistd.h>

#include<netdb.h>

#include<fcntl.h>    

#include"./remoteShell.h"

#define COMMAND_LEN 100


void handle_command(int client_fd,char* command) {

    pid_t pid = fork();
    switch (pid)
    {
    case -1:
        fprintf(stderr,"Fork failed\n");
        exit(EXIT_FAILURE);
        break;
    case 0:
        char* tkn = strtok(command," ");
        int index = 0;
        char* whole_command[COMMAND_LEN];
        while (tkn != NULL) {
            
            whole_command[index++] = tkn;

            tkn = strtok(NULL," ");
        }
        whole_command[index] = NULL;

        dup2(client_fd, STDOUT_FILENO);
        close(client_fd);  
        execvp(whole_command[0],whole_command);
        
        exit(EXIT_FAILURE);
        break;
        
    }

    wait(NULL);
    printf("Child process ended\n");

}


void interpret(int client_fd) {
    char buffer[BUFSIZ];
    ssize_t bytes_read = read(client_fd,buffer,BUFSIZ-1);
    while (bytes_read > 0) {
        buffer[bytes_read] = '\0';
        printf("Client entered command: %s\n",buffer);
        handle_command(client_fd,buffer);

        bytes_read = read(client_fd,buffer,BUFSIZ-1);
    }
    
    if(bytes_read == -1) {
        fprintf(stderr,"%s\n",strerror(errno));
        exit(EXIT_FAILURE);
    }
    printf("Client disconnected\n");
}



int main(int argc,char** argv) {

    bool running = false;
    int client_fd = -1;
    struct sockaddr_in client_address,address;
    socklen_t client_len = sizeof(client_address);
    if(argc < 3) {
        fprintf(stderr,"Invalid use\n");
        exit(EXIT_FAILURE);
    }
    int port = atoi(argv[2]);
    char* ip_address = argv[1];

    int server_fd = init_server(&address,AF_INET,port,ip_address,SOCK_STREAM);
    running = start_server(&address,server_fd);

    while (running) {
        
        client_fd = accept(server_fd,(struct sockaddr*)&client_address,&client_len);
        if(client_fd < 0) {
            fprintf(stderr,"Could not accept client\n");
            exit(EXIT_FAILURE);
        }
        printf("Client connected\n");

        interpret(client_fd);
        close(client_fd);


    }
    



    exit(EXIT_SUCCESS);
}
