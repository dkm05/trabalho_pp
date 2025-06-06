#pragma once

#define HASHMAP_SIZE 4000
#define MACRO_SIZE 4096

typedef struct Macro{
        char id[MACRO_SIZE];
        char value[MACRO_SIZE];
        int disponibilidade;
        int simples;
        char ** parametros;
        int qtd_param;
}Macro;

enum {
    OCUPADO = 0,
    LIVRE = 1,
};

void inicializar(const char *arquivo);
int hash(const char str[]);
int acharposicao(const char *arquivo, const char id[]);
void inserir(const char *arquivo, Macro a);
int buscar(const char *arquivo, const char id[], int *ret);
void die(const char *str);
