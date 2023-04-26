#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <fcntl.h>
#include <sys/types.h> /* for pid_t */
#include <sys/wait.h> /* for wait */

int main(){
    static char *argv[]={NULL}; 
    char comando[10];
    fgets(comando, 10, stdin);
    comando[strcspn(comando, "\n")] = 0;
    printf("%saaa", comando);
    int x = execvp(comando, argv);
    return 0;
}