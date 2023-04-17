#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

int main(){
    int id = fork();
    //aula 2: processo pai espera o processo filho executar
    int n;
    if(id == 0){
        n = 1;
    } else {
        n = 6;
    }
    wait();
    int i;
    for (i = n; i < n + 5; i++){
        printf("%d ", i);
        fflush(stdout);
    }
    printf("\n");

    //aula 1: basico
    /*if (id == 0){
        printf("Hello from child process\n");
    }
    else{
        printf("Hello from parent process\n");
    }*/

    return 0;
}