#include<unistd.h>
#include<string.h>
#include<stdio.h>
#include<stdlib.h>
#include<sys/time.h>
#include<sys/types.h>




char* time_read(int timeout) {

    static char buffer[BUFSIZ];
    fd_set rdfs;
    struct timeval time;
    int ret;
    FD_ZERO(&rdfs);
    FD_SET(0,&rdfs);
    bzero(&time,sizeof(struct timeval));
    time.tv_sec = timeout;

    ret = select(1,&rdfs,0,0,&time);
    if(ret && FD_ISSET(0,&rdfs)) {
        memset(buffer,0,BUFSIZ);
        ssize_t read_bytes = read(STDIN_FILENO,buffer,BUFSIZ-1);
        if(read_bytes > 0 ) {
            :
        }
    }



}


int main(int argc,char** argv) {





    exit(EXIT_SUCCESS);
}
