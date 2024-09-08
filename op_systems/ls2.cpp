#include<stdlib.h>
#include<stdio.h>
#include<dirent.h>
#include<sys/stat.h>
#include<string.h>
#include<string>

struct OutputFormat {

    bool printAll = false;
    bool printSize = false;
    bool printMode = false;
    bool printTime = false;
    char outputBuf[255];

};

const char* check_dir(char* name) {
    if(name[0] == '-' || strstr(name,".")) {
        return ".";
    }

    return name;
}

bool is_valid(char* str,char** input,int len) {
    for(int i = 0; i < len;i++) {
        if(strstr(input[i],".")) {
            if(!strcmp(input[i],str)) {
                return true;
            }
        }
    }

    return false;
   
}


void init_format(int len,char** input,OutputFormat * format) {
    for(int i = 0; i < len;i++) {
        if(!strcmp(input[i],"-s")) format->printSize = true;
        if(!strcmp(input[i],"-m")) format->printMode = true;
        if(!strcmp(input[i],"-a")) format->printAll = true;
        if(!strcmp(input[i],"-t")) format->printTime = true;
        
    }
}

int main(int argc,char** argv) {
    if(argc < 2) {
        fprintf(stderr,"Invalid ls format\n");
        exit(EXIT_FAILURE);
    }

    std::string directory(check_dir(argv[1]));
    OutputFormat format;
    init_format(argc,argv,&format);
    struct dirent* entry; 
    struct stat stats;
    DIR* selected_directory = NULL;

    selected_directory = opendir(directory.c_str());
    if(!selected_directory) {
        fprintf(stderr,"Invalid directory\n");
        exit(EXIT_FAILURE);
    }


    while ((entry = readdir(selected_directory))) {
        if(is_valid(entry->d_name,argv,argc) || format.printAll){
             
             (void)stat(entry->d_name,&stats);

        }
       
    }
    

    closedir(selected_directory);

    return EXIT_SUCCESS;
}