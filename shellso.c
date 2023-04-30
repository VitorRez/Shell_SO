#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <fcntl.h>
#include <sys/types.h> /* for pid_t */
#include <sys/wait.h> /* for wait */
#include "argv.h"
#include "auxiliar.h"

void executa_arquivo(char* comando, char** argv, int background){
    pid_t pid = fork();
    if(pid==0){
        execvp(comando, argv);
        exit(127);
    }else{
        if(background == 0){
            waitpid(pid,0,0);
        }
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

void pipeline(char** command, int posicao_i, int posicao_f, int out, int background){
    int first = 0;
    int last = 0;
    int in = 0;
    int out = 0;
    /*for(int i = 0; i < size; i++){
        if(strcmp(command[i], "|") == 0){
            if(first == 0){
                first = i;
            }
            last = i;
        }
    }*/
    for(int i = posicao_i; i <= posicao_f; i++){
        for(int k = i; i < )
    }
}

int Pipe(char* process_l, char* process_r, char** argv_l, char ** argv_r, int out, int in, int background){
    int fd[2];
    int saved_stdout;

    /*if(pipe(fd) == -1){
        return 1;
    }*/

    int pid1 = fork();
    /*if(pid1 < 0){
        return 2;
    }*/

    if(pid1 == 0){
        dup2(fd[1], 1);
        close(fd[0]);
        close(fd[1]);
        if(in != 0){
            int saved_stdin = dup(0);
            dup2(in, 0);
            close(out);
        }
        execvp(process_l, argv_l);
        if(int != 0){
            dup2(saved_stdin, 0);
            close(saved_stdout);
        }
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

    if(background == 0){
        wait(NULL);
        wait(NULL);
    }
    return out;
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
    int background = 0;
    if(strcmp(command_parsed[num_linhas-1], "&") == 0){
        background = 1;
    }
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
        int z = Pipe(argv_l[0], argv_r[0], argv_l, argv_r, out, background);
        //delete_argv(argv_l, posicao_e);
        //delete_argv(argv_r, (posicao_s-posicao_p - 1));
    }else{
        char** argv = cria_argv(posicao_e);
        preenche_argv(argv, command_parsed, posicao_e, 0);
        executa_arquivo(command_parsed[0], argv, background);
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