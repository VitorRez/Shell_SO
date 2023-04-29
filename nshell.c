#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <fcntl.h>
#include <sys/types.h> /* for pid_t */
#include <sys/wait.h> /* for wait */

//Funções de apoio, seja para interpretação de comandos 
//e processamento de strings

char** processar_string(char* text, char* separador, int cont){
    char** mat = (char**)malloc(cont*sizeof(char*));
    for(int i = 0; i < cont; i++){
        mat[i] = (char*)malloc(100*sizeof(char));
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

char** cria_argv(int size){
    char** argv = (char**)malloc((size+1)*sizeof(char*));
    for(int i = 0; i < size+1; i++){
        argv[i] = (char*)malloc(100*sizeof(char));
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
    for(int i = 0; i < size+1; i++){
        free(argv[i]);
    }
    free(argv);
}

//Funções essenciais para o funcionamento do shell
void executa_arquivo(char* comando, char** argv){
    pid_t pid = fork();
    if(pid==0){
        execvp(comando, argv);
        exit(127);
    }else{
        waitpid(pid,0,0);
    }
}

int redirect_out(char* filename){
    int saved_stdout = dup(1);
    int out = open(filename, O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR);
    dup2(out, 1);
    close(out);
    return(saved_stdout);
}

void return_out(int saved_stdout){
    dup2(saved_stdout, 1);
    close(saved_stdout);
}

int redirect_in(char* filename){
    int x = procura_arquivo(filename);
    if(x == 0){
        return -1;
    }else{
        int saved_stdin = dup(0);
        int in = open(filename, O_RDONLY);
        dup2(in, 0);
        close(in);
        return saved_stdin;
    }
}

int return_in(int saved_stdin){
    dup2(saved_stdin, 0);
    close(saved_stdin);
}

int Pipe(char* process_l, char* process_r, char** argv_l, char ** argv_r, int out){
    int fd[2];
    int saved_stdout;

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
        if(out != 0){
            int saved_stdout = dup(1);
            dup2(out, 1);
            close(out);
        }
        execvp(process_r, argv_r);
        if(out != 0){
            dup2(saved_stdout, 1);
            close(saved_stdout);
        }
    }

    close(fd[0]);
    close(fd[1]);

    wait(NULL);
    wait(NULL);
    return 0;
}

//

void caminhar_nos_comandos(char* command_line){
    int num_linhas = get_num_lines(command_line);
    char** command_parsed = processar_string(command_line, " ", num_linhas);
    int posicao_e = num_linhas;
    int posicao_p;
    int posicao_s = num_linhas;
    int e = 0;
    int s = 0;
    int p = 0;
    int saved_stdin = 0;
    int saved_stdout = 1;
    int out = 0;
    for(int i = 0; i < num_linhas; i++){
        if(strcmp(command_parsed[i], "<=") == 0){
            posicao_e = i;
            e = 1;
            int x = procura_arquivo(command_parsed[i+1]);
            if(x == 0) return;
            saved_stdin = redirect_in(command_parsed[i+1]);
            if(saved_stdin == -1) return;
        }
        if(strcmp(command_parsed[i], "|") == 0){
            posicao_p = i;
            p = 1;
        }
        if(strcmp(command_parsed[i], "=>") == 0){
            posicao_s = i;
            s = 1;
            saved_stdout = redirect_out(command_parsed[i+1]);
            out = open(command_parsed[i+1], O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR);
        }
    }
    if(p == 1){
        if(posicao_e == num_linhas){
            posicao_e = posicao_p;
        }
        char** argv_l = cria_argv(posicao_e);
        preenche_argv(argv_l, command_parsed, posicao_e, 0);
        char** argv_r = cria_argv(posicao_s-posicao_p);
        preenche_argv(argv_r, command_parsed, (posicao_s-posicao_p - 1), posicao_p+1);
        int z = Pipe(argv_l[0], argv_r[0], argv_l, argv_r, out);
        //delete_argv(argv_l, posicao_e);
        //delete_argv(argv_r, (posicao_s-posicao_p - 1));
    }else{
        char** argv = cria_argv(posicao_e);
        preenche_argv(argv, command_parsed, posicao_e, 0);
        executa_arquivo(command_parsed[0], argv);
    }
    if(e == 1){
        return_in(saved_stdin);
    }
    if(s == 1){
        return_out(saved_stdout);
    }
    /*for(int i = 0; i < num_linhas; i++){
        free(command_parsed[i]);
    }
    free(command_parsed);*/
}

//

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

/*void main(){
    char filename[100];
    fgets(filename, 100, stdin);
    filename[strcspn(filename, "\n")] = 0;
    int x = procura_arquivo(filename);
    printf("%d\n", x);
}*/