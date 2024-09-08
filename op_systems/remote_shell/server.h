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
#include<errno.h>
#include<string>
#include<vector>

class Server {

private:
    int server_fd;
    int client_fd;
    char* address;
    char buffer[BUFSIZ];
    uint32_t port;
    struct sockaddr_in server_address;
    struct sockaddr_in client_address;
    bool server_running;

    void init_address(int domain,char* address,uint32_t port,struct sockaddr_in* socket_address);
    void handle_shell(char* shell_command);
public:

    Server(int domain,char* address, int port);
    void start();


    
};
