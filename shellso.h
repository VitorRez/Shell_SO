#ifndef SHELLSO_HEADER
#define SHELLSO_HEADER

void executa_arquivo(char*, char**);
int redirect_out(char*);
void return_out(int);
int redirect_in(char*);
void return_in(int);
int Pipe(char*, char*, char**, char**, int);
void caminhar_nos_comandos(char*);

#endif