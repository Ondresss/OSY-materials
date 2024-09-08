#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<arpa/inet.h>
#include<sys/socket.h>
#include <string.h>
#include<netdb.h>
#include<errno.h>



int main(int argc,char ** argv) {

    FILE* read_fp = NULL;
    char buffer[BUFSIZ+1];
    int chars_read;

    memset(buffer,0,sizeof(buffer));
    read_fp = popen("uname -a","r");
    if(read_fp != NULL) {
        chars_read = fread(buffer,sizeof(char),BUFSIZ,read_fp);
        if(chars_read > 0) {
            printf("Vystup byl: %s\n",buffer);
        }
        pclose(read_fp);
        exit(EXIT_SUCCESS);
    } else {
        write(2,"popen failed\n",12);
        exit(EXIT_FAILURE);
    }

    return EXIT_SUCCESS;
}