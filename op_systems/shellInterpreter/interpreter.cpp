#include<unistd.h>
#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<string.h>
#include<string>
#include<fcntl.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<errno.h>

#define MAX_COMMAND_LEN 100
#define PIPING_SIZE 1024 * 1024
#define FILENAME 500

void exit_error(int num) {
    printf("%s\n",strerror(errno));
    exit(EXIT_FAILURE);
}


void handle_command(char* command,char* buffer) {
    int c_to_p_pipe[2];
    if(pipe(c_to_p_pipe) == -1) {
        exit_error(errno);
    }
    pid_t pid = fork();
    int status = -1;
    switch (pid)
    {
    case -1:
        exit_error(errno);
        break;
    case 0:
        close(c_to_p_pipe[0]);
        char* shellcom[MAX_COMMAND_LEN];
        int counter = 0;
        bool end = false;
        if(strstr(command,">")) {
            char* tkn = strtok(command," ");
            char filename[FILENAME];
            close(c_to_p_pipe[1]);
            while (tkn != NULL) {
                if(end) {
                    strcpy(filename,tkn);
                    break;
                }
                if((!strcmp(tkn,">"))) {
                    end = true;
                    tkn = strtok(NULL," ");
                    continue;
                }
                shellcom[counter++] = tkn;
                tkn = strtok(NULL," ");
            }
            shellcom[counter] = NULL;
            int r_fd = open(filename,O_CREAT | O_APPEND | O_WRONLY );
            (void)dup2(r_fd,STDOUT_FILENO);
            close(r_fd);
            execvp(shellcom[0],shellcom);

            exit(EXIT_FAILURE);
        }
        char* tkn = strtok(command," ");
        while (tkn != NULL) {
            shellcom[counter++] = tkn;
            tkn = strtok(NULL," ");
        }
        shellcom[counter] = NULL;
        (void)dup2(c_to_p_pipe[1],STDOUT_FILENO);
        close(c_to_p_pipe[1]);
        execvp(shellcom[0],shellcom);

        exit(EXIT_FAILURE);
        break;
    
    }
    close(c_to_p_pipe[1]);
    if(read(c_to_p_pipe[0],buffer,PIPING_SIZE-1) < 0) {
        printf("Didnt read from pipe\n");
        memset(buffer,'\0',PIPING_SIZE-1);
    }

    wait(&status);
    close(c_to_p_pipe[0]);
    printf("Child exit\n");
}


void handle_command_read(char* command, char* data) {
    
    int p_to_c_pipe[2];
    int c_to_p_pipe[2];
    if(pipe(p_to_c_pipe) == -1) {
        exit_error(errno);
    }
    if(pipe(c_to_p_pipe) == -1) {
        exit_error(errno);
    }
    pid_t pid = fork();
    switch (pid)
    {
    case -1:
        exit_error(errno);
        break;
    case 0:
        close(p_to_c_pipe[1]);
        close(c_to_p_pipe[0]);
        char* shellcom[MAX_COMMAND_LEN];
        int counter = 0;
        char* tkn = strtok(command," ");
        while (tkn != NULL) {
            shellcom[counter++] = tkn;
            tkn = strtok(NULL," ");
        }
        shellcom[counter] = NULL;
        (void)dup2(p_to_c_pipe[0],STDIN_FILENO);
        (void)dup2(c_to_p_pipe[1],STDOUT_FILENO);
        close(p_to_c_pipe[0]);
        close(c_to_p_pipe[1]);

        execvp(shellcom[0],shellcom);

        printf("Could not execute\n");
        exit(EXIT_FAILURE);
        break;
    
    }


    close(p_to_c_pipe[0]);
    close(c_to_p_pipe[1]);

    if(write(p_to_c_pipe[1],data,strlen(data)) == -1) exit_error(errno);
    close(p_to_c_pipe[1]);
    memset(data,'\0',PIPING_SIZE-1);
    if(read(c_to_p_pipe[0],data,PIPING_SIZE-1) < 0) {
        printf("Could not read\n");
        exit_error(errno);
    }
    wait(NULL);
    close(c_to_p_pipe[0]);
    printf("Child from piping exited\n");

}

void interpret(char* input) {

    char output[PIPING_SIZE];
    memset(output,'\0',PIPING_SIZE);
    int executed_count = 0;
    char* tkn = strtok(input,"|");
    while (tkn != NULL) {
        if(!executed_count) {
            handle_command(tkn,output);
        } else {
            handle_command_read(tkn,output);
        }

        executed_count++;

        tkn = strtok(NULL,"|");
    }
    printf("%s",output);
    
}


int main(int argc,char**argv) {

    char buffer[BUFSIZ];

    while (true) {
            (void)write(STDOUT_FILENO,"$ ",2);
            size_t read_no_bytes = read(STDIN_FILENO,buffer,BUFSIZ-1);
            if( read_no_bytes < 0) {
                printf("Could not read the stdin\n");
                exit_error(errno);
            }
            buffer[read_no_bytes-1] = '\0';
            if(!strcmp(buffer,"exit")) break;
            interpret(buffer);


    }
    

    return 0;
}