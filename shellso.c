#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h> /* for pid_t */
#include <sys/wait.h> /* for wait */

const char* remove_enter(char* nome){
    int size = strlen(nome);
    char *copia = (char*)malloc((size-1)*sizeof(char));
    for(int i = 0; i < size; i++){
        if(i == 0){
            copia[i] = '.';
        }
        if(i == 1){
            copia[i] = '/';
        }
        if(nome[i] != '\n'){
            copia[i] = nome[i];
        }
    }
    return copia;
}

int procura_arquivo(const char* nome){
    FILE* file;

    if ((file = fopen(nome, "r"))){
        fclose(file);
        return 1;
    }
    printf("%s: comando não encontrado\n", nome);
    return 0;
}

void executa_arquivo(const char* diretorio, char** argv){
    pid_t pid = fork();
    if(pid==0){
        execv(diretorio, argv);
        exit(127);
    }else{
        waitpid(pid,0,0);
    }
}

int main(int argc, char **argv){
    char *prompt = "(Diga la) $";
    char text[100];
    const char *n_text;
    size_t n = 0;

    (void)argc;
    (void)argv;

    do{
        printf("%s ", prompt);
        fgets(text, 100, stdin);
        if(strcmp(text, "fim\n") == 0) break;
        n_text = remove_enter(text);
        int x = procura_arquivo(n_text);
        if(x == 1){
            static char *argv[]={"echo","Foo is my name.",NULL};
            executa_arquivo(n_text, argv);
        }
    }while(strcmp(text, "fim\n") != 0);

    return 0;
}