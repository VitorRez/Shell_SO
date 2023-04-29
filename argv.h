#ifndef HEADER_ARGV
#define HEADER_ARGV

char** cria_argv(int);
void insere(char**, int, char*);
void preenche_argv(char**, char**, int, int);
void delete_argv(char**, int);

#endif