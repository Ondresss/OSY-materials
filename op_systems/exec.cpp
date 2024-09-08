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
    
    char* command[] = {"grep","-E","c$","-",0};
    char* bin_file = command[0];

    execvp(bin_file,command);



    return EXIT_SUCCESS;
}