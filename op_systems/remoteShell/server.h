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
#define MAX_CLIENTS 10
#define MAX_OPEN_FILES 25
class Client {

private:
    struct sockaddr_in address;
    int sock_fd;
    socklen_t address_len;

public:
   inline Client() : sock_fd(-1),address_len(sizeof(struct sockaddr_in)) {
        bzero(&this->address,sizeof(struct sockaddr_in));
   }
   inline ~Client() {
        close(this->sock_fd);
   }
   inline void set_socket(int new_sock) {this->sock_fd = new_sock;}
   inline int get_socket() const {return this->sock_fd;} 
   inline struct sockaddr_in* get_address() {return &this->address;}
   inline socklen_t* get_len() {return &this->address_len;} 
};

class Server {

private:
    int socked_fd;
    int domain;
    int type;
    struct sockaddr_in address;
    int port;
    char IP[INET_ADDRSTRLEN];
    bool running;
protected:
    Client* connected_clients[MAX_CLIENTS];
    void init();
    void start();
    inline virtual void run() {
        printf("running base server class\n");
    }
public:
    inline Server(int domain,int type,int port,char* ip_address) : domain(domain),type(type),port(port),running(true) {
        strcpy(IP,ip_address);
        IP[strlen(ip_address)] = '\0';
        this->init();
        this->start();

    }

    inline int get_socket_fd() const {return this->socked_fd; }
    inline int get_domain() const {return this->domain;}
    inline int get_type() const {return this->type; }
    inline const struct sockaddr_in* get_address() {return &this->address;}
    inline int get_port() const {return this->port;}
    inline bool is_running() const {return this->running;}



};

class RemoteShellServer : protected Server {

private:

    int open_file_fds[MAX_OPEN_FILES];
    pthread_t th_ids[MAX_CLIENTS];
    int current_no_clients;



public:

    inline RemoteShellServer(int domain,int type,int port,char* ip_address) : Server(domain,type,port,ip_address), current_no_clients(0) {
        bzero(open_file_fds,sizeof(open_file_fds));
    }
    virtual void run();


};