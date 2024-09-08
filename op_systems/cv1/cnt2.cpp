#include<unistd.h>
#include<stdlib.h>
#include<stdio.h>
#include<string.h>
char BUFFER[BUFSIZ];


int count_chars() {
    int count = 0;
    ssize_t read_bytes = read(STDIN_FILENO,BUFFER,BUFSIZ-1);
    while(read_bytes > 0) {
        BUFFER[read_bytes] = '\0';
        count+=strlen(BUFFER);
        read_bytes = read(STDIN_FILENO,BUFFER,BUFSIZ-1);
    }    
    if(read_bytes == -1) {
        perror("Could not read from standard input\n");
        exit(EXIT_FAILURE);
    }
    return count;
}


int count_words() {

    int count = 0;
    ssize_t read_bytes = read(STDIN_FILENO,BUFFER,BUFSIZ-1);
    while(read_bytes > 0) {
        BUFFER[read_bytes] = '\0';
        char* tkn = strtok(BUFFER," ");
        while(tkn) {
            count++;
            tkn = strtok(NULL," ");
        }
        read_bytes = read(STDIN_FILENO,BUFFER,BUFSIZ-1);
    }    
    if(read_bytes == -1) {
        perror("Could not read from standard input\n");
        exit(EXIT_FAILURE);
    }

    return count;

}


int count_lines() {
    int count = 0;
    ssize_t read_bytes = read(STDIN_FILENO,BUFFER,BUFSIZ-1);
    while(read_bytes > 0) {
        BUFFER[read_bytes] = '\0';
        count++; 
        read_bytes = read(STDIN_FILENO,BUFFER,BUFSIZ-1);
    }    
    if(read_bytes == -1) {
        perror("Could not read from standard input\n");
        exit(EXIT_FAILURE);
    }

    return count;
}


int main(int argc,char** argv) {

    char c = -1;
    int count = -1;

    while ((c = getopt(argc,argv,"cwl")) != -1){
        switch (c)
        {
        case 'c':
            count = count_chars();
            break;
        case 'w':
            count = count_words();
            break;
        case 'l':
            count = count_lines();
            break;
        default:
            perror("Wrong option\n");
            exit(EXIT_FAILURE);
            break;
        }
    }
    printf("%d\n",count);
    exit(EXIT_SUCCESS);
}