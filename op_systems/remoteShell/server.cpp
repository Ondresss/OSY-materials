#include"./server.h"


void* handle_shell_client(void* args) {
    char BUFFER[BUFSIZ+1];
    Client* client = (Client*)args;
    ssize_t read_bytes_client = read(client->get_socket(),BUFFER,BUFSIZ);
    int c_to_p_pipe[2];
    if(pipe(c_to_p_pipe) == -1) {
        fprintf(stderr,"Could not create pipe existing client thread\n");
        delete client;
        pthread_exit(NULL);
    }
    while(read_bytes_client > 0) {
        BUFFER[read_bytes_client] = '\0';
        if(!strcmp(BUFFER,"exit")) {
            printf("Client exiting via exit\n");
            pthread_exit(NULL);
        }
        pid_t pid = fork();
        switch (pid)
        {
        case -1:
            fprintf(stderr,"Could not create child process\n");
            exit(EXIT_FAILURE);
            break;
        case 0:
            close(c_to_p_pipe[0]);
            printf("Child was created\n");
            printf("Command to execute is: %s\n",BUFFER);
            char* command[BUFSIZ];
            int index = 0;
            char* tkn = strtok(BUFFER," ");
            while(tkn) {
                command[index++] = tkn;
                printf("command[%d]= %s\n",index-1,tkn);
                tkn = strtok(NULL," ");
            }
            command[index] = NULL;
            printf("Command after parsing %s\n",command[0]);
            (void)dup2(c_to_p_pipe[1],STDOUT_FILENO);
            execvp(command[0],command);
            perror("Error occured while executing command\n");
            exit(EXIT_FAILURE);
            break;    
        }
        bzero(BUFFER,BUFSIZ+1);
        close(c_to_p_pipe[1]);
        ssize_t bytes_from_child = read(c_to_p_pipe[0],BUFFER,BUFSIZ);
        if(bytes_from_child == -1) {
            perror("Could not read output from child proccess\n");
            pthread_exit(NULL);
        }
        BUFFER[bytes_from_child] = '\0';
        printf("Read from child: %s",BUFFER);
        write(client->get_socket(),BUFFER,strlen(BUFFER));

        int status = -1;
        wait(&status);

        read_bytes_client = read(client->get_socket(),BUFFER,BUFSIZ);
    }
    if(read_bytes_client == -1) {
        fprintf(stderr,"Could not read from the client\n");
        pthread_exit(NULL);
    }
    printf("Client disconnected\n");

    pthread_exit(NULL);
}


void Server::init() {
    bzero(&this->address,sizeof(struct sockaddr_in));
    this->address.sin_port = htons(this->port);
    this->address.sin_family = this->domain;
    if(inet_pton(this->domain,this->IP,&this->address.sin_addr) == -1) {
        fprintf(stderr,"Error occured while writing address to struct\n");
        exit(EXIT_FAILURE);

    }

}

void Server::start() {
    this->socked_fd = socket(this->domain,this->type,0);
    if(this->socked_fd == -1) {
        fprintf(stderr,"Error occured while creating a socket\n");
        exit(EXIT_FAILURE);
    }
    if(bind(this->socked_fd,(struct sockaddr*)&this->address,sizeof(this->address)) == -1) {
        fprintf(stderr,"Could not bind socket\n");
        exit(EXIT_FAILURE);
    }
    if(listen(this->socked_fd,SOMAXCONN) == -1) {
        fprintf(stderr,"Could not start listening\n");
        exit(EXIT_FAILURE);
    }

}

void RemoteShellServer::run() {
        while (this->is_running()) {
            printf("Server is waiting for client\n");
            Client* new_client = new Client();
            int client_fd = accept(this->get_socket_fd(),(struct sockaddr*)new_client->get_address(),new_client->get_len());
            if(client_fd < 0) {
                fprintf(stderr,"Could not accept client\n");
                delete new_client;
                continue;
            }
            new_client->set_socket(client_fd);
            this->current_no_clients++;
            printf("Client was accepted\n");
            if(pthread_create(&this->th_ids[this->current_no_clients-1],NULL,handle_shell_client,(void*)new_client) != 0) {
                fprintf(stderr,"Could not create a new thread\n");
                delete new_client;
                continue;
            }
            

    }
    for(ssize_t i = 0; i < this->current_no_clients;i++) {
        if(pthread_join(this->th_ids[i],NULL) != 0) {
            fprintf(stderr,"Could not wait for client\n");
            exit(EXIT_FAILURE);
        }
    }
}