#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <fcntl.h>
#include <sys/types.h> /* for pid_t */
#include <sys/wait.h> /* for wait */
#include "shellso.h"

void main(int argc, char** argv){
    char* prompt = "(Diga la) $";
    char text[100];

    (void)argc;
    (void)argv;

    do{
        printf("%s ", prompt);
        fgets(text, 100, stdin);
        if(strcmp(text, "fim\n") == 0) break;
        text[strcspn(text, "\n")] = 0;
        caminhar_nos_comandos(text);
    }while(strcmp(text, "fim\n") != 0);
}