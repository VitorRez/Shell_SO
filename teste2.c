#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(){
    char text[100];
    fflush(stdin);
    scanf("%s", text);
    fflush(stdin);
    printf("%s\n", text);
    return 0;
}