#include"./server.h"

int main(int argc,char** argv) {


    int port = atoi(argv[2]);
    char* ip = argv[1];
    {
        RemoteShellServer remote_shell(AF_INET,SOCK_STREAM,port,ip);
        remote_shell.run();
    }



    exit(EXIT_SUCCESS);
}