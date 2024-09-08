#include "server.h"




Server::Server(int domain,char* address, int port) {

    this->client_fd = -1;
    this->server_fd = socket(domain,SOCK_STREAM,0);
    if(this->server_fd == -1) {
        fprintf(stderr,"Could not create socket\n");
        exit(EXIT_FAILURE);
    }

    this->address = address;
    this->port = port;
    this->server_running = false;
    memset(this->buffer,0,BUFSIZ);
    this->init_address(domain,this->address,this->port,&this->server_address);
}

void Server::init_address(int domain,char* address,uint32_t port,struct sockaddr_in* socket_address) {

    bzero(&socket_address->sin_addr,sizeof(*socket_address));
    socket_address->sin_family = domain;
    socket_address->sin_port = htons(port);
    
    if(inet_pton(AF_INET,address,&socket_address->sin_addr) == -1) {
        fprintf(stderr,"%s\n",strerror(errno));
        exit(EXIT_FAILURE);
    }


}


void Server::start() {

    if(bind(this->server_fd,(struct sockaddr*)&this->server_address,sizeof(this->server_address)) == -1) {
        fprintf(stderr,"%s\n",strerror(errno));
        exit(EXIT_FAILURE);
    }

    if(listen(this->server_fd,SOMAXCONN) == -1) {
        fprintf(stderr,"%s\n",strerror(errno));
        exit(EXIT_FAILURE);
    }

    this->server_running = true;
    printf("Server is running\n");
    socklen_t client_len = sizeof(this->client_address); 

    while (this->server_running){
        printf("Server is waiting for client\n");
        this->client_fd = accept(this->server_fd,(struct sockaddr*)&this->server_address,&client_len);
        if(this->client_fd == -1) {
            fprintf(stderr,"%s\n",strerror(errno));
            this->server_running = false;
        }
        printf("Client was accepted\n");
        int bytes_read = read(this->client_fd,this->buffer,BUFSIZ-1);
        while(bytes_read > 0) {
            this->buffer[bytes_read] = '\0';

            this->handle_shell(this->buffer);


            bytes_read = read(this->client_fd,this->buffer,BUFSIZ-1);
        }

        printf("Client disconnected\n");
        close(this->client_fd);
    }
    
    printf("Server stopped running\n");


}


void Server::handle_shell(char* shell_command) {


    /*std::vector<std::string> shell;

    char* token = strtok(shell_command," ");
    while (token != NULL)
    {   
        std::string s(token);
        shell.push_back(s);
        token = strtok(NULL," ");
    }

    pid_t proc_id = fork();
    switch (proc_id)
    {
    case -1:
        fprintf(stderr,"Could not create child process\n");
        this->server_running = false;
        break;
    case 0:
        (void)dup2(this->client_fd,1);
        execlp()
        break;
    
    default:
        break;
    }*/

   shell_command[strlen(shell_command)-1] = '\0';
   for(int i = 0; i < strlen(shell_command);i++) {
        if(shell_command[i] == '\n'){
            printf("Yas");
        }
   }
   printf("%s\n",shell_command);
   FILE* shell = popen(shell_command,"r");
   if(shell == NULL) {
        fprintf(stderr,"Could not interpret the shell\n");
   }
   while (fgets(this->buffer,BUFSIZ-1,shell))
   {
        printf("%s",this->buffer);
   }
   
}

int main(int argc,char** argv) {


    struct sockaddr_in server_address,client_address;
    char* address = argv[1];
    uint32_t port = atoi(argv[2]);

    Server server(AF_INET,address,port);
    server.start();

    return EXIT_SUCCESS;
}






