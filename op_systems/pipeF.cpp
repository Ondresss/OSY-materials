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

    int data_processed = -1;
    int file_pipes[2];
    const char* some_data = "ahoj bro";
    char buffer[BUFSIZ + 1];
    pid_t fork_result = -1;

    memset(buffer,'\0',sizeof(buffer));

    if(pipe(file_pipes) == 0) {
        fork_result = fork();
        if(fork_result == -1) {
            fprintf(stderr,"Fork failed\n");
            exit(EXIT_FAILURE);
        }
        if(!fork_result) {
            data_processed = read(file_pipes[0],buffer,BUFSIZ);
            buffer[data_processed] = '\0';
            printf("Data read: %s\n",buffer);
            exit(EXIT_SUCCESS);
        } else {

            data_processed = write(file_pipes[1],some_data,strlen(some_data));

        }



    }
    return EXIT_SUCCESS;
}