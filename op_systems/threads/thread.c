#include<unistd.h>
#include<pthread.h>
#include<stdlib.h>
#include<netdb.h>
#include<arpa/inet.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<string.h>
#include<stdio.h>
#include<netinet/in.h>
#include <errno.h>

void* routine(void* arguments) {

    printf("Test from1\n");
    sleep(3);
    printf("Test2\n");
}

int main(int argc,char** argv) {

    pthread_t t1,t2;

    pthread_create(&t1,NULL,routine,NULL);
    pthread_create(&t2,NULL,routine,NULL);
    pthread_join(t1,NULL);
    pthread_join(t2,NULL);

    return EXIT_SUCCESS;
}