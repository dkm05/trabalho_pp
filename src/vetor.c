#include "vetor.h"
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>

struct Vetor {
        int n;
        int capacidade;
        char* dados;
};

Vetor* novo_vetor(int capacidade){
        if (capacidade <= 0)
                return NULL;
        Vetor* V = malloc(sizeof(Vetor));
        V->n = 0;
        V->capacidade = capacidade;
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

void vetor_clear(Vetor *V)
{
        V->n = 0;
}

// precisamos adicionar em qualquer posição...
void adiciona_elemento(Vetor* V, char x, int i){
        if (i < 0)
                return;
        if (V->n == V->capacidade) {
                V->capacidade *= 2;
                V->dados = realloc(V->dados, V->capacidade*sizeof(V->dados));
        }
        V->dados[i] = x;
        if (i == V->n)
                V->n++;
}

/* observar essa função */
void remove_elemento(Vetor* V, int i){
        if(i >= 0 && i < V->n){
                V->dados[i] = '\0';
                V->n--;
        }
}

/* memcpy retorna dest, por enquanto nao fazemos isso 
 *
 * pode ser interessante criar uma função que retorna
 * a capacidade do vetor
 */
void vetor_memcpy(Vetor *dest, const Vetor *src, int offset, int count) {
        if (!dest || !src)
                return;
        if (count > src->capacidade)
                return;
        /* improvável nesse caso...*/
        if (count > dest->capacidade) {
                dest->capacidade *= 2;
                dest->dados = realloc(dest->dados, dest->capacidade * sizeof(dest->dados));
        }
        /* difícil, mas é bom fazer algo */
        if (!memcpy(dest->dados, src->dados + offset - count, count))
                exit(EXIT_FAILURE);
        dest->n = count;
}


void
organize_buffer(Vetor *V)
{
        int pos = 0;
        int len = tamanho_vetor(V);
        char c;
        for (int i = 0; i < len; i++) {
                c = elemento(V, i);
                if (c != '\0') {
                        adiciona_elemento(V, c, pos);
                        pos++;
                        if (i != pos - 1)
                                adiciona_elemento(V, '\0', i);
                }
        }
        V->n = pos;
}
