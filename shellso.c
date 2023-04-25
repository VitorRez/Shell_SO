#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <fcntl.h>
#include <sys/types.h> /* for pid_t */
#include <sys/wait.h> /* for wait */

int procura_arquivo(const char* nome){
    FILE* file;

    if ((file = fopen(nome, "r"))){
        fclose(file);
        return 1;
    }
    printf("%s: comando não encontrado\n", nome);
    return 0;
}

void executa_arquivo(char* diretorio, char** argv){
    pid_t pid = fork();
    if(pid==0){
        execvp(diretorio, argv);
        exit(127);
    }else{
        waitpid(pid,0,0);
    }
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

char** identifica_pipes(char* text){
    char** mat = (char**)malloc(2*sizeof(char*));
    for(int i = 0; i < cont; i++){
        mat[i] = (char*)malloc(100*sizeof(char));
    }
    char* token = strtok(text, "|");
    int cont = 0;
    while(token != NULL){
        mat[cont] = token;
        token = strtok(NULL, "|");
        cont++;
    }
    return mat;
}

char** identifica_redirect_in(char* text){
    char** mat = (char**)malloc(2*sizeof(char*));
    for(int i = 0; i < cont; i++){
        mat[i] = (char*)malloc(100*sizeof(char));
    }
    char* token = strtok(text, "<=");
    int cont = 0;
    while(token != NULL){
        mat[cont] = token;
        token = strtok(NULL, "<=");
        cont++;
    }
    return mat;
}

char** identifica_redirect_in(char* text){
    char** mat = (char**)malloc(2*sizeof(char*));
    for(int i = 0; i < cont; i++){
        mat[i] = (char*)malloc(100*sizeof(char));
    }
    char* token = strtok(text, "<=");
    int cont = 0;
    while(token != NULL){
        mat[cont] = token;
        token = strtok(NULL, "<=");
        cont++;
    }
    return mat;
}

char** processar_string(char* text, char* separador, int cont){
    char** mat = (char**)malloc(cont*sizeof(char*));
    for(int i = 0; i < cont; i++){
        mat[i] = (char*)malloc(100*sizeof(char));
    }
    //separa os tokens
    char* token = strtok(text, separator);
    cont = 0;
    while(token != NULL){
        mat[cont] = token;
        token = strtok(NULL, separator);
        cont++;
    }
    return mat;
}

void comando(char* command, char** argv){
    executa_arquivo(command, argv);
    return;
}

void comando_pipe(char* command, char** argv){
    execvp(command, argv);
    return;
}

int redirect_out(const char* filename){
    int saved_stdout = dup(1);
    int out = open(filename, O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR);
    dup2(out, 1);
    close(out);
    return saved_stdout;
}

void return_out(int saved_stdout){
    dup2(saved_stdout, 1);
    close(saved_stdout);
}

int redirect_in(const char* filename){
    int saved_stdin = dup(0);
    int in = open(filename, O_RDONLY);
    dup2(in, 0);
    close(in);
    return saved_stdin;
}

int return_in(int saved_stdin){
    dup2(saved_stdin, 0);
    close(saved_stdin);
}

void redirect_inNout(char* text, char* filein, char* fileout, char** argv){
    int saved_stdin = redirect_in(filein);
    int saved_stdout = redirect_out(fileout);
    executa_arquivo(text, argv);
    return_in(saved_stdin);
    return_out(saved_stdout);
}


int Pipe(char* process_l, char* process_r, char** argv){
    int fd[2];

    if(pipe(fd) == -1){
        return 1;
    }

    int pid1 = fork();
    if(pid1 < 0){
        return 2;
    }

    if(pid1 == 0){
        dup2(fd[1], 1);
        close(fd[0]);
        close(fd[1]);
        execvp(command, argv);
    }

    int pid2 = fork();
    if(pid2 < 0){
        return 3;
    }

    if(pid2 == 0){
        dup2(fd[0], 0);
        close(fd[0]);
        close(fd[1]);
        execvp(command, argv);
    }

    close(fd[0]);
    close(fd[1]);

    wait(NULL);
    wait(NULL);
    /*waitpid(pid1, NULL, 0);
    waitpid(pid2, NULL, 0);*/
    return 0;
}

void identificar_comandos(char** commands, int size){
    static char *argv[]={NULL}; 
    switch(size){
        case 0:
            executa_arquivo(commands[0], argv);
            break;
        case 2:
            if(strcmp(commands[1], "=>") == 0){
                int saved_stdout = redirect_out(commands[2]);
                executa_arquivo(commands[0], argv);
                return_out(saved_stdout);
                break;
            }
            if(strcmp(commands[1], "<=") == 0){
                int saved_stdin = redirect_in(commands[0]);
                executa_arquivo(commands[2], argv);
                return_in(saved_stdin);
                break;
            }
            if(commands[1][0] == '|'){
                int x = Pipe(commands[0], commands[2], argv);
                break;
            }
        case 4:
            if(strcmp(commands[1], "<=") == 0 && strcmp(commands[3], "=>") == 0){
                redirect_inNout(commands[2], commands[0], commands[4], argv);
                break;
            }
        Default:
            printf("Arquivo nao encontrado\n");
    }
}

int main(int argc, char **argv){
    char* prompt = "(Diga la) $";
    char text[100];
    char** str_a;
    size_t n = 0;
    int size;

    (void)argc;
    (void)argv;

    do{
        printf("%s ", prompt);
        fgets(text, 100, stdin);
        if(strcmp(text, "fim\n") == 0) break;
        text[strcspn(text, "\n")] = 0;
        size = get_num_lines(text);
        str_a = processar_string(text);
        identificar_comandos(str_a, size-1);
    }while(strcmp(text, "fim\n") != 0);
    return 0;
}