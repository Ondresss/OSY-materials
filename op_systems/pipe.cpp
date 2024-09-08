#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<arpa/inet.h>
#include<sys/socket.h>
#include <string.h>
#include<netdb.h>
#include<errno.h>
#include<sys/stat.h>
#include<fcntl.h>

int main(int argc,char ** argv) {
    int fd = -1;
    const char* message = "ahoj";
    if(mkfifo("myfifo",0777) == -1) {
        fprintf(stderr,"Could not create fifo file\n");
    }

    fd = open("myfifo",O_WRONLY);
    if(write(fd,message,strlen(message)) == -1) {
        fprintf(stderr,"%s\n",strerror(errno));
        exit(EXIT_FAILURE);
    }
    close(fd);





    return EXIT_SUCCESS;
}