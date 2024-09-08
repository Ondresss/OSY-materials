#include "./remoteShell.h"
int init_server(struct sockaddr_in * address,int domain,int port,char* ip_address,int type) {
    
    int fd = -1;
    bzero(address,sizeof(*address));
    address->sin_family = domain;
    address->sin_port = htons(port);
    if(inet_pton(domain,ip_address,&address->sin_addr) == -1) {
        fprintf(stderr,"%s\n",strerror(errno));
        exit(EXIT_FAILURE);
    }

    fd = socket(domain,type,0);

    if(fd < 0) {
        fprintf(stderr,"%s\n",strerror(errno));
        exit(EXIT_FAILURE);
    }

    return fd;

}


bool start_server(struct sockaddr_in * address,int server_fd) {
    if(bind(server_fd,(struct sockaddr*)address,sizeof(*address)) == -1 ) {
        fprintf(stderr,"%s\n",strerror(errno));
        exit(EXIT_FAILURE);
    }

    if(listen(server_fd,MAX_CONS) == -1) {
        fprintf(stderr,"%s\n",strerror(errno));
        exit(EXIT_FAILURE);
    }

    return true;
}
