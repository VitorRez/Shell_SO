#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv){
    char *prompt = "(Diga la) $";
    char text[100];
    size_t n = 0;

    (void)argc;
    (void)argv;

    do{
        printf("%s ", prompt);
        fgets(text, 100, stdin);
        printf("%s", text);
    }while(strcmp(text, "fim\n") != 0);

    return 0;
}