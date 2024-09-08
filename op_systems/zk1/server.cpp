#include<stdlib.h>
#include<stdio.h>
#include<semaphore.h>
#include<netinet/in.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<sys/types.h>
#include<string.h>
#include<pthread.h>
#define MAX_CLIENTS 15
#define NUM_OF_OPERATORS 4

size_t global_counter = 10;
sem_t mutex;

class Client {

private:
    int socket_fd;
    struct sockaddr_in* address;
    char IP[INET_ADDRSTRLEN + 1];
    socklen_t len;

public:
    inline Client(int sock) : socket_fd(sock),len(0) {
        address = (struct sockaddr_in*)malloc(sizeof(struct sockaddr_in));
        memset(IP,'\0',INET_ADDRSTRLEN+1);
    }
    inline struct sockaddr_in* get_address() {return this->address; }
    inline char* get_ip() { return this->IP;}
    inline int get_socket() {return this->socket_fd;};
    inline void set_socket (int new_sock) {this->socket_fd = new_sock; }
    inline socklen_t* get_len() {return &this->len;}
};

class Server {

private:
    int socket_fd;
    struct sockaddr_in* address;
    char IP[INET_ADDRSTRLEN+1];
    Client* clients[MAX_CLIENTS];
    pthread_t th_ids[MAX_CLIENTS];
    int no_clients;
    int port;
    int domain;
    int type;
    int protocol;
    bool running;
public:
    inline Server(char* ip_address,int port,int domain,int type, int protocol) : port(port), domain(domain),type(type),protocol(protocol),running(false),no_clients(0) {
        strcpy(IP,ip_address);
        IP[strlen(IP)] = '\0';
        this->address = nullptr;
        this->init_server();
        memset(&this->clients,0,MAX_CLIENTS * sizeof(Client*));
    }
    inline ~Server() {
        free(this->address);
        for(int i = 0; i < this->no_clients;i++) delete this->clients[i];
    }
    void init_server();
    void start();

};
void* handle_client(void* args) {
    char BUFFER[BUFSIZ+1];
    memset(BUFFER,'\0',BUFSIZ);
    Client* client = (Client*)args;
    ssize_t read_bytes = read(client->get_socket(),BUFFER,BUFSIZ);
    int counter = 0;
    while (read_bytes > 0) {
        if(!counter) {
            sem_wait(&mutex);
        }
        if(counter >= 10) {
            sem_post(&mutex);
            counter = 0;
        }
        BUFFER[read_bytes] = '\0';
        const char* operators[NUM_OF_OPERATORS] = {"*","+","-","\\"};
        printf("Read from client %s\n",BUFFER);
        for(int i = 0; i < NUM_OF_OPERATORS;i++) {
            if(strstr(BUFFER,operators[i])) {
                char* left_operand = strtok(BUFFER,operators[i]);
                char* right_operand = strtok(NULL,operators[i]);
                if(!strcmp(operators[i],"*")) {
                    int result = atoi(left_operand) * atoi(right_operand);
                    sprintf(BUFFER,"%d",result);
                    printf("Interpred as: %d\n",result);
                    write(client->get_socket(),BUFFER,strlen(BUFFER));
                }
                break;
            }
        }
        counter++;
        read_bytes = read(client->get_socket(),BUFFER,BUFSIZ);
    }
    if(read_bytes == -1) {
        fprintf(stderr,"Error while reading\n");
        exit(EXIT_FAILURE);
    }
    printf("Client disconnected\n");
    client->set_socket(-1); 
    pthread_exit(NULL);
}


void Server::init_server() {
    this->address = (struct sockaddr_in*)malloc(sizeof(struct sockaddr_in));
    bzero(address,sizeof(struct sockaddr_in));
    this->address->sin_family = domain;
    this->address->sin_port = htons(port);
    if(inet_pton(domain,this->IP,&this->address->sin_addr) == -1) {
        fprintf(stderr,"Could not write address\n");
        exit(EXIT_FAILURE);
    }



}

void Server::start() {
    this->socket_fd = socket(this->domain,this->type,this->protocol);
    if(this->socket_fd < 0 ) {
        fprintf(stderr,"Could not create socket\n");
        exit(EXIT_FAILURE);
    }

    if(bind(this->socket_fd,(struct sockaddr*)this->address,sizeof(*address)) == -1) {
        fprintf(stderr,"Could not bind\n");
        exit(EXIT_FAILURE);
    }

    if(listen(this->socket_fd,SOMAXCONN) == -1) {
        fprintf(stderr,"Could not listen\n");
        exit(EXIT_FAILURE);
    }

    printf("Server started listening\n");
    this->running = true;

    while (this->running) {
        printf("Server is waiting for client\n");
        this->clients[this->no_clients] = new Client(-1);
        int new_sock = accept(this->socket_fd,(struct sockaddr*)this->clients[this->no_clients]->get_address(),this->clients[this->no_clients]->get_len());
        if(new_sock < 0) {
            fprintf(stderr,"Could not accept the client\n");
            exit(EXIT_FAILURE);
        }
        this->clients[this->no_clients]->set_socket(new_sock);
        printf("Client connected\n");
        if(pthread_create(&this->th_ids[this->no_clients],NULL,handle_client,(void*)this->clients[this->no_clients]) == -1) {
            fprintf(stderr,"Could not create thread\n");
            exit(EXIT_FAILURE);
        }
        this->no_clients++;
    }
    

    for(int i = 0; i < this->no_clients;i++) {
        if(pthread_join(this->th_ids[i],NULL) == -1) {
            fprintf(stderr,"Could not join main thread\n");
            exit(EXIT_FAILURE);
        }
    }

}



int main(int argc,char** argv) {

    sem_init(&mutex,0,1);
    char* ip_address = argv[1];
    int port = atoi(argv[2]);
    {
        Server server(ip_address,port,AF_INET,SOCK_STREAM,0);
        server.start();
    }
    sem_destroy(&mutex);

    exit(EXIT_SUCCESS);
}