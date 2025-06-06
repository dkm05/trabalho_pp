#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hashmap.h"

void inicializar(const char *arquivo) {
    FILE *arq = fopen(arquivo, "rb");
    // se existe, assume que tem dados do registro
    if (!arq) {
        arq = fopen(arquivo, "w+b");
        Macro a;
        a.disponibilidade = LIVRE;
        for (int i = 0; i < HASHMAP_SIZE; i++) {
            fwrite(&a, sizeof(Macro), 1, arq);
        }
    }
    fclose(arq);
}
int hash(const char str[]) {
    int k=0,key=0;
    while(str[k]!='\0'){
        key += (int)str[k];
    }
    return (key % HASHMAP_SIZE);
}
int acharposicao(const char *arquivo, const char id[]) {
    int pos = hash(id);
    Macro a;
    FILE *arq = fopen(arquivo, "rb");
    if (!arq)
        die("Não foi possível abrir o arquivo.\n");
    fseek(arq, pos * sizeof(Macro), SEEK_SET);
    fread(&a, sizeof(Macro), 1, arq);
    while (a.disponibilidade == OCUPADO) {
        pos = (pos + 1) % HASHMAP_SIZE;
        fseek(arq, pos * sizeof(Macro), SEEK_SET);
        fread(&a, sizeof(Macro), 1, arq);
    }
    fclose(arq);
    return pos;
}
void inserir(const char *arquivo, Macro a) {
    int pos = acharposicao(arquivo, a.id);
    FILE *arq = fopen(arquivo, "r+b");
    if (!arq)
        die("Não foi possível abrir o arquivo.\n");
    fseek(arq, pos * sizeof(Macro), SEEK_SET);
    fwrite(&a, sizeof(Macro), 1, arq);
    fclose(arq);
}
int buscar(const char *arquivo, const char id[], int *ret) {
    int pos = hash(id);
    Macro a;
    FILE *arq = fopen(arquivo, "rb");
    if (!arq)
        die("Não foi possível abrir o arquivo.\n");
    fseek(arq, pos * sizeof(Macro), SEEK_SET);
    fread(&a, sizeof(Macro), 1, arq);
    while (strcmp(a.id,id) && a.disponibilidade == OCUPADO) {
        pos = (pos + 1) % HASHMAP_SIZE;
        fseek(arq, pos * sizeof(Macro), SEEK_SET);
        fread(&a, sizeof(Macro), 1, arq);
    }
    if (a.disponibilidade == LIVRE) {
        fclose(arq);
        return 0;
    }
    *ret = pos;
    fclose(arq);
    return 1;
}
