#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <fcntl.h>
#include <sys/types.h> /* for pid_t */
#include <sys/wait.h> /* for wait */

void print_dir(){
    char cwd[1024];
    getcwd(cwd, sizeof(cwd));
    printf("%s\n", cwd);
}

void print_files(){
    DIR *dr;
    struct dirent *en;
    dr = opendir(".");
    if (dr) {
        while ((en = readdir(dr)) != NULL) {
            printf("%s ", en->d_name);
        }
        printf("\n");
        closedir(dr);
    }
}

const char* remove_enter(char* nome){
    int size = strlen(nome);
    char *copia = (char*)malloc((size-1)*sizeof(char));
    for(int i = 0; i < size; i++){
        if(nome[i] != '\n'){
            copia[i] = nome[i];
        }
    }
    return copia;
}

const char* exe(char* text){
    const char* copia = remove_enter(text);
    int size = strlen(copia);
    char *executavel = (char*)malloc((size+2)*sizeof(char));
    executavel[0] = '.';
    executavel[1] = '/';
    for(int i = 2; i < size+2; i++){
        executavel[i] = copia[i-2];
    }
    return executavel;
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

int get_num_lines(char* text){
    int cont = 1;
    for(int i = 0; i < strlen(text); i++){
        if(text[i] == ' '){
            cont++;
        }
    }
    return cont;
}

char** processar_string(char* text){
    //conta quantos tokens serão feitos
    int cont = get_num_lines(text);
    //cria um array de strings (array com os tokens)
    char** mat = (char**)malloc(cont*sizeof(char*));
    for(int i = 0; i < cont; i++){
        mat[i] = (char*)malloc(100*sizeof(char));
    }
    //separa os tokens
    char* token = strtok(text, " ");
    cont = 0;
    while(token != NULL){
        mat[cont] = token;
        token = strtok(NULL, " ");
        cont++;
    }
    return mat;
}

void redirect_out(const char* text, char* filename){
    int x = procura_arquivo(text);
    if(x == 1){
        static char *argv[]={"echo","Foo is my name.",NULL};
        int saved_stdout = dup(1);
        int out = open(filename, O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR);
        dup2(out, 1);
        executa_arquivo(text, argv);
        dup2(saved_stdout, 1);
        close(saved_stdout);
    }
}

void redirect_in(const char* text, char* filename){
    int x = procura_arquivo(text);
    if(x == 1){
        static char *argv[]={"echo","Foo is my name.",NULL};
        int saved_stdin = dup(STDIN_FILENO);
        int in = open(filename, O_RDONLY);
        dup2(in, STDIN_FILENO);
        executa_arquivo(text, argv);
        dup2(saved_stdin, STDIN_FILENO);
        close(saved_stdin);
    }
}

void redirect_inNout(const char* text, char* filein, char* fileout){
    int x = procura_arquivo(text);
    if(x == 1){
        static char *argv[]={"echo","Foo is my name.",NULL};
        int saved_stdin = dup(STDIN_FILENO);
        int saved_stdout = dup(1);
        int in = open(filein, O_RDONLY);
        int out = open(fileout, O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR);
        dup2(in, STDIN_FILENO);
        dup2(out, 1);
        executa_arquivo(text, argv);
        dup2(saved_stdin, STDIN_FILENO);
        dup2(saved_stdout, 1);
        close(saved_stdin);
        close(saved_stdout);
    }
}


int Pipe(const char* process_l, const char* process_r){
    int fd[2];
    static char *argv[]={"echo","Foo is my name.",NULL};

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
        execv(process_l, argv);
    }

    int pid2 = fork();
    if(pid2 < 0){
        return 3;
    }

    if(pid2 == 0){
        dup2(fd[0], 0);
        close(fd[0]);
        close(fd[1]);
        execv(process_r, argv);
    }

    close(fd[0]);
    close(fd[1]);

    waitpid(pid1, NULL, 0);
    waitpid(pid2, NULL, 0);
    return 0;
}

void identificar_comandos(char** commands, int size){
    switch(size){
        case 0:
            if(strcmp(commands[0], "pwd\n") == 0){
                print_dir();
                break;
            }
            if(strcmp(commands[0], "ls\n") == 0){
                print_files();
                break;
            }
            if(commands[0][0] == '.' && commands[0][1] == '/'){
                const char* n_text = remove_enter(commands[0]);
                static char *argv[]={"echo","Foo is my name.",NULL};
                executa_arquivo(n_text, argv);
                break;
            }else{
                const char* n_text = remove_enter(commands[0]);
                procura_arquivo(n_text);
                break;
            }
        case 2:
            if(strcmp(commands[1], "=>") == 0){
                const char* n_text = remove_enter(commands[0]);
                redirect_out(n_text, commands[2]);
            }
            if(strcmp(commands[1], "<=") == 0){
                const char* n_text = remove_enter(commands[2]);
                redirect_in(n_text, commands[0]);
            }
            if(commands[1][0] == '|'){
                const char* n_text_l = remove_enter(commands[0]);
                const char* n_text_r = remove_enter(commands[2]);
                int n = Pipe(n_text_l, n_text_r);
            }
            break;
        case 4:
            if(strcmp(commands[1], "<=") == 0 && strcmp(commands[3], "=>") == 0){
                const char* n_text = remove_enter(commands[2]);
                redirect_inNout(n_text, commands[0], commands[4]);
                return;
            }
            break;
        default:
            printf("comando não encontrado\n");
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
        size = get_num_lines(text);
        str_a = processar_string(text);
        identificar_comandos(str_a, size-1);
    }while(strcmp(text, "fim\n") != 0);
    return 0;
}