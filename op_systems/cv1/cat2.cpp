#include<unistd.h>
#include<stdlib.h>
#include<stdio.h>
#include<fcntl.h>

/*Vytvořte program, který se bude chovat identicky jako program ‘cat’. Otestujte funkci například zadáním ‘./cat /etc/passwd’. Program v případě zadání neexistujícího souboru vypíše program chybu na chybový výstup.

Vytvořte program cnt, tento program bude vyžadovat jeden ze tří parametrů -c, -w, -l.
Při zadání parametrů -c program na standardní výstup vypíše počet znaků (včetně mezer) v textu, která obdržel na standardním vstupu.
Při zadání parametrů -w program na standardní výstup vypíše počet slov v textu, která obdržel na standardním vstupu.
Při zadání parametrů -l program na standardní výstup vypíše počet řádků v textu, která obdržel na standardním vstupu. Program nebude započítávat prázdné řádky.
*/
void run_command(int len,char** files) {
    char BUFFER[BUFSIZ];
    int fd = -1;
    ssize_t bytes_read = -1;
    for(int i = 1; i < len;i++) {
        if(access(files[i],F_OK) == -1) {
            perror("File doesnt exist\n");
            exit(EXIT_FAILURE);
        }
        fd = open(files[i],O_RDONLY,0666);
        if(fd < 0) {
            perror("File could be opened\n");
            exit(EXIT_FAILURE);
        }
        bytes_read = read(fd,BUFFER,BUFSIZ-1);
        while (bytes_read > 0) {
            BUFFER[bytes_read] = '\0';
            printf("%s",BUFFER);

            bytes_read = read(fd,BUFFER,BUFSIZ-1);
        }
        close(fd);
    }

}



int main(int argc,char** argv) {

    if(argc == 1) {
        fprintf(stderr,"Please specifify files\n");
        exit(EXIT_FAILURE);
    }

    run_command(argc,argv);



    exit(EXIT_FAILURE);
}