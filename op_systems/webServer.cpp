#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<pthread.h>
#include<netdb.h>
#include<unistd.h>
#include<vector>
#include<sys/wait.h>
#define MAX_CONS 10
#define COMMAND_LEN 256
struct Client {
    int client_fd;
    struct sockaddr_in* address;
    socklen_t len;
};

struct Server {
    int sock_fd;
    int domain;
    int type;
    int protocol;
    struct sockaddr_in* address;
    int port;
    bool running;
    int no_clients;
};

std::vector<Client*> clients;
const char* response_format2 =
    "HTTP/1.1 %d %s\r\n"
    "Server: %s\r\n"
    "Content-Length: %d\r\n"
    "Content-Type: text/html\r\n"
    "Connection: Closed\r\n"
    "\r\n"
    "%s";

const char* response_format =
    "HTTP/1.1 %d %s\r\n"
    "Server: %s\r\n"
    "Content-Length: %d\r\n"
    "Content-Type: text/html\r\n"
    "Connection: Closed\r\n"
    "\r\n"
    "%s";
const char* minimal_response =
    "HTTP/1.1 %d %s\r\n"
    "Content-Length: %d\r\n"
    "Connection: Closed\r\n"
    "\r\n%s";

Server* create_server(int domain,int type,int protocol,const char* address,int port) {

    struct sockaddr_in* server_address = (struct sockaddr_in*)malloc(sizeof(struct sockaddr_in));
    bzero(server_address,sizeof(struct sockaddr_in));
    server_address->sin_family = domain;
    server_address->sin_port  = htons(port);  
    if(inet_pton(domain,address,&server_address->sin_addr) == -1) {
        fprintf(stderr,"Could not load address\n");
        exit(EXIT_FAILURE);
    }

    Server* server = new Server;
    server->domain = domain;
    server->type = type;
    server->protocol = protocol;
    server->address = server_address; 
    server->port = port;
    server->sock_fd = -1;
    server->sock_fd = socket(domain,type,protocol);
    if(server->sock_fd < 0) {
        fprintf(stderr,"Could not create socket\n");
        exit(EXIT_FAILURE);
    }

    server->running = true;
    server->no_clients = 0;
    return server;
}


void handle_request(char* request,int fd) {
    char machineName[BUFSIZ];
    int res = gethostname(machineName,BUFSIZ-1);
    if(res == -1) {
        fprintf(stderr,"Could not get host name\n");
        exit(EXIT_FAILURE);
    }
    int c_to_p_pipe[2];
    int status = -1;
    if(pipe(c_to_p_pipe) == -1) {
        fprintf(stderr,"Could not create pipe channel\n");
        exit(EXIT_FAILURE);
    }

    char output[BUFSIZ];
    char* tkn = strtok(request,"\n");
    char* header = strdup(tkn);
    if(!header) {
        perror("Could not get header\n");
        exit(EXIT_FAILURE);
    }
    printf("%s\n",header);
    pid_t pid = fork();
    if(pid == -1) {
        fprintf(stderr,"Could not fork parent\n");
        exit(EXIT_FAILURE);
    }
    if(!pid) {
        close(c_to_p_pipe[0]);
        char* command[COMMAND_LEN];
        char* request_type = strtok(header," ");
        printf("Request: %s\n",request_type);
        char* directory = strtok(NULL," ");
        printf("Directory: %s\n",directory);
        char* command_type = strstr(strdup(directory),"/") + 1;
        printf("Command to execute: %s\n",command_type);
        char* cmd_to_parse = strtok(command_type,"*");
        if(!cmd_to_parse) {
            puts("Wroong\n");
        }
        int index = 0;
        while(cmd_to_parse) {

            command[index++] = cmd_to_parse;

            printf("%s\n",command[index-1]);
            cmd_to_parse = strtok(NULL,"*");
        }
        command[index] = NULL;

        printf("%s",command[0]);
        (void)dup2(c_to_p_pipe[1],STDOUT_FILENO);

        
        execvp(command[0],command);
        fprintf(stderr,"Executing failed\n");
        exit(EXIT_FAILURE);

    }

    close(c_to_p_pipe[1]);
    ssize_t read_bytes = read(c_to_p_pipe[0],output,BUFSIZ-1);
    output[read_bytes] = '\0';
    if(read_bytes < 0 ) {
        memset(output,'\0',BUFSIZ);
        sprintf(output,response_format,404,"Not Found",machineName,20,"<h1>Not found</h1>");
        write(fd,output,strlen(output));
    }
    sprintf(output,response_format2,200,"OK",machineName,strlen(output),output);
    puts("Response sent\n");
    write(fd,output,strlen(output));
    wait(&status);

    close(c_to_p_pipe[0]);

    free(header);

}



void* handle_client(void* arg) {
    char buffer[BUFSIZ];
    int fd = *((int*)arg);
    ssize_t read_bytes = read(fd,buffer,BUFSIZ-1);
    while(read_bytes > 0 ) {
        buffer[read_bytes] = '\0';
        handle_request(buffer,fd);

        read_bytes = read(fd,buffer,BUFSIZ-1);
    }

    if(read_bytes == -1) {
        perror("Could not read from the client\n");
        exit(EXIT_FAILURE);
    }
    printf("Client disconnected\n");

    pthread_exit(NULL);
}



void start_server(Server* server) {
    if(bind(server->sock_fd,(struct sockaddr*)server->address,sizeof(*server->address)) == -1) {
        fprintf(stderr,"Could not bind the socket\n");
        exit(EXIT_FAILURE);
    }
    if(listen(server->sock_fd,MAX_CONS) == -1) {
        fprintf(stderr,"Could not listen\n");
        exit(EXIT_FAILURE);
    }
    
    printf("Web server is listening on port: %d\n",server->port);

    pthread_t ids[MAX_CONS];
    


    while(server->running) {
        if(server->no_clients > MAX_CONS) {
            puts("Could not acept more clients\n");
            exit(EXIT_FAILURE);
        }
        Client* client = new Client;
        client->client_fd = -1;
        client->address = (struct sockaddr_in*)malloc(sizeof(struct sockaddr_in));
        client->len = sizeof(*client->address);

        client->client_fd = accept(server->sock_fd,(struct sockaddr*)client->address,&client->len);
        if(client->client_fd < 0) {
            perror("Could not accept the client\n");
            exit(EXIT_FAILURE);
        }

        puts("Client was accepted\n");
        if(pthread_create(&ids[server->no_clients],NULL,handle_client,&client->client_fd) != 0) {
            perror("Could not create a new thread\n");
            exit(EXIT_FAILURE);
        }
        clients.push_back(client);

        server->no_clients++;
    }

    for(int i = 0; i < server->no_clients;i++) {
        if(pthread_join(ids[i],NULL) != 0) {
            perror("Could not join main thread with other threads\n");
            exit(EXIT_FAILURE);
        }
    }
    for(auto client : clients) {
        close(client->client_fd);
        delete client;
    }


}
int main(int argc,char** argv){ 


    int port = atoi(argv[2]);
    const char* address = argv[1];
    Server* server = create_server(AF_INET,SOCK_STREAM,0,address,port);
    start_server(server);

    delete server;


    exit(EXIT_SUCCESS);

}
