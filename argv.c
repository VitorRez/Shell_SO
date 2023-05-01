#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <fcntl.h>
#include <sys/types.h> /* for pid_t */
#include <sys/wait.h> /* for wait */

char** cria_argv(int size){
    char** argv = (char**)malloc((size+1)*sizeof(char*));
    char linha[100];
    for(int i = 0; i < size+1; i++){
        argv[i] = linha;
    }
    return argv;
}

void insere_argv(char** argv, int posicao, char* comando){
    argv[posicao] = comando;
}

void preenche_argv(char** argv, char** comando, int size, int posicao){
    for(int i = 0; i < size+1; i++){
        insere_argv(argv, i , comando[i+posicao]);
    }
    argv[size] = NULL;
}

void delete_argv(char** argv, int size){
    //for(int i = 0; i < size+1; i++){
        //printf("%s\n", argv[i]);
        //free(argv[i]);
    //}
    free(argv);
}