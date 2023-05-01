#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <fcntl.h>
#include <sys/types.h> /* for pid_t */
#include <sys/wait.h> /* for wait */

char** processar_string(char* text, char* separador, int cont){
    char** mat = (char**)malloc(cont*sizeof(char*));
    char str[100];
    for(int i = 0; i < cont; i++){
        mat[i] = str;
    }
    char* token = strtok(text, separador);
    cont = 0;
    while(token != NULL){
        mat[cont] = token;
        token = strtok(NULL, separador);
        cont++;
    }
    return mat;
}

int get_num_lines(char* text){
    int cont = 1;
    for(int i = 0; i < strlen(text); i++){
        if(text[i] == ' '){
            cont++;
        }
    }
    return cont;
} 

int procura_arquivo(char* nome){
    FILE* file;

    if ((file = fopen(nome, "r"))){
        fclose(file);
        return 1;
    }
    printf("%s: comando não encontrado\n", nome);
    return 0;
}