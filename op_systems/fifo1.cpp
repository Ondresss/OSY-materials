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

#define FILE_PATH "myfifo"


int main(int argc,char ** argv) {
    
    int pipe_fd;
    int res;
    int open_mode = O_WRONLY;
    int bytes_sent = 0;
    char buffer[BUFSIZ];

    if(access(FILE_PATH,F_OK) == -1){
        res = mkfifo(FILE_PATH,0777);
        if(res != 0) {
            fprintf(stderr,"Nelze vytvorit FIFO soubor\n");
            exit(EXIT_FAILURE);
        }

        
    }


    return EXIT_SUCCESS;
}