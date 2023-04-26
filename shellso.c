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

char** processar_string(char* text, char* separador, int cont){
    char** mat = (char**)malloc(cont*sizeof(char*));
    for(int i = 0; i < cont; i++){
        mat[i] = (char*)malloc(100*sizeof(char));
    }
    //separa os tokens
    char* token = strtok(text, separador);
    cont = 0;
    while(token != NULL){
        mat[cont] = token;
        token = strtok(NULL, separador);
        cont++;
    }
    return mat;
}

char** cria_argv(char** C, int size){
    char** argv = (char**)malloc(size*sizeof(char*));
    for(int i = 0; i < size; i++){
        argv[i] = (char*)malloc(100*sizeof(char));
    }
    for(int i = 0; i < size-1; i++){
        argv[i] = C[i+1];
    }
    return argv;
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

int Pipe(char* process_l, char* process_r, char** argv_l, char ** argv_r){
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
        execvp(process_l, argv_l);
    }

    int pid2 = fork();
    if(pid2 < 0){
        return 3;
    }

    if(pid2 == 0){
        dup2(fd[0], 0);
        close(fd[0]);
        close(fd[1]);
        execvp(process_r, argv_r);
    }

    close(fd[0]);
    close(fd[1]);

    wait(NULL);
    wait(NULL);
    return 0;
}

int verifica_pipe(char* text){
    int size = strlen(text);
    for(int i = 0; i < size; i++){
        if(text[i] == '|') return 1;
    }
    return 0;
}

int verifica_entrada_arquivo(char* text){
    int size = strlen(text);
    for(int i = 0; i < size-1; i++){
        if(text[i] == '<' && text[i+1] == '=') return 1;
    }
    return 0;
}

int verifica_saida_arquivo(char* text){
    int size = strlen(text);
    for(int i = 0; i < size-1; i++){
        if(text[i] == '=' && text[i+1] == '>') return 1;
    }
    return 0;
}

void identificar_comandos(char* commands){
    int size = strlen(commands);
    int p_red_e = 0;
    int p_red_s = 0;
    int p_pip = 0;
    for(int i = 0; i < size-1; i++){
        if(commands[i] == '<' && commands[i+1] == '=') p_red_e = i;
        if(commands[i] == '|') p_pip = i;
        if(commands[i] == '=' && commands[i+1] == '>') p_red_s = i;
    }
    for(int i = 0; i < p_red_e; i++) printf("%c", commands[i]);
    printf("\n");
    for(int i = p_red_e+3; i < p_pip; i++) printf("%c", commands[i]);
    printf("\n");
    for(int i = p_pip+2; i < p_red_s; i++) printf("%c", commands[i]);
    printf("\n");
    for(int i = p_red_s+3; i < size; i++) printf("%c", commands[i]);
    printf("\n");
}

/*void identificar_comandos(char* commands){
    int p = verifica_pipe(commands);
    int e = verifica_entrada_arquivo(commands);
    int s = verifica_saida_arquivo(commands);
    if(p == 1){
        char** P = processar_string(commands, "|", 2);
        if(e == 1){
            char** E = processar_string(P[0], "<=", 2);
            size1 = get_num_lines(E[0]);
            size2 = get_num_lines(E[1]);
            char** C1 = processar_string(E[0], " ", size1);
            char** C2 = processar_string(E[1], " ", size2);
            if(size1 > 0){
                char** C1 = processar_string(E[0], " ", size1);
                char** argv1 = cria_argv(C1, size1);
            }else{
                char** argv1 = {NULL};
            }
            if(size2 > 0){
                char** C2 = processar_string(E[1], " ", size2);
                char** argv2 = cria_argv(C2, size2);
            }else{
                char** argv2 = {NULL};
            }
            int saved_stdin = redirect_in(C1[0]);
            
        }
        if(e == 0){

        }
        if(s == 1){
            char** S = processar_string(P[1], "=>", 2);
        }
        if(s == 0){

        }
    }
    if(p == 0){
        if(e == 1){

        }
        if(e == 0){

        }
        if(s == 1){

        }
        if(s == 0){
            
        }
    }
}*/

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
        //str_a = processar_string(text);
        identificar_comandos(text);
    }while(strcmp(text, "fim\n") != 0);
    return 0;
}