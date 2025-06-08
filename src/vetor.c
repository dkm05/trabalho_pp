#include "vetor.h"
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

struct Vetor {
        int n;
        int capacidade;
        char* dados;
};

Vetor* novo_vetor(){
        Vetor* V = malloc(sizeof(Vetor));
        V->n = 0;
        V->capacidade = 1024;
        V->dados = calloc(V->capacidade, sizeof(char));
        return V;
}

void finaliza_vetor(Vetor* V){
        free(V->dados);
        free(V);
}

int tamanho_vetor(const Vetor* V){
        return V->n;
}

char elemento(const Vetor* V, int i){
        return V->dados[i];
}

// precisamos adicionar em qualquer posição...
void adiciona_elemento(Vetor* V, char x, int i){
        if (i < 0)
                return;
        if (V->n == V->capacidade) {
                V->capacidade *= 2;
                V->dados = realloc(V->dados, V->capacidade*sizeof(float));
        }
        V->dados[i] = x;
        if (i == V->n)
                V->n++;
}

void remove_elemento(Vetor* V, int i){
        if(i >= 0 && i < V->n){
                V->dados[i] = '\0';
                V->n--;
        }
}

void vetor_memcpy(Vetor *dest, const Vetor *src, int ini, int fim) {
        if (!dest || !src)
                return;
        /*
        // improvável nesse caso...
        if (fim > tamanho_vetor(dest)) {
                dest->capacidade *= 2;
                dest->dados = realloc(dest->dados, dest->capacidade*sizeof(float));
        }
        */
        if (fim > tamanho_vetor(src))
                return;
        memcpy(dest->dados, src->dados + ini - fim, fim);
}

void
organize_buffer(Vetor *str)
{
        int pos = 0;
        int len = tamanho_vetor(str);
        char c;
        for (int i = 0; i < len; i++) {
                c = elemento(str, i);
                if (c != '\0') {
                        adiciona_elemento(str, c, pos);
                        pos++;
                        if (i != pos - 1)
                                adiciona_elemento(str, '\0', i);
                }
        }
        str->n = pos;
}
